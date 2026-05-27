#include <SPI.h>
#include <Ethernet.h>

const int SENSOR_PIN = A0;

//Faixa do sensor real
const float TENSAO_MIN = 1.0;
const float TENSAO_MAX = 5.0;

//Tempo entre leituras
const unsigned long INTERVALO_LEITURA = 500;

unsigned long ultimaLeitura = 0;

//MAC da shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

//IP do servidor
IPAddress server(192, 168, 0, 200);

EthernetClient client;

float lerTensaoFiltrada() {

  float soma = 0;
  
  //média de 10 leituras
  for (int i = 0; i<10; i++) {
  
    int adc = analogRead(SENSOR_PIN);
    
    float tensao = adc * (5.0 / 1023.0);
    
    soma += tensao;
    delay(5);
  
  }
  
  return soma / 10.0;
}

//Converter tensão em nível
float calcularNivel(float tensao) {

  if(tensao < TENSAO_MIN)
 	tensao = TENSAO_MIN;
  
  if(tensao > TENSAO_MAX)
    tensao = TENSAO_MAX;
    
  //converter 1-5v em 0-100%
  float nivel = ((tensao - TENSAO_MIN) /
                 (TENSAO_MAX - TENSAO_MIN)) * 100.0;
  return nivel;
}

//converter corrente em tensão
float calcularCorrente(float tensao) {
  
  float corrente =
    4.0 + (
      (tensao - TENSAO_MIN) *
      (16.0 / (TENSAO_MAX - TENSAO_MIN))
    );
  return corrente;
}

//logs
void exibirLeitura(
  int adc, 
  float tensao, 
  float corrente,
  float nivel
) {

  Serial.print("[ADC] ");
  Serial.println(adc);
  
  Serial.print("[Tensao] ");
  Serial.print(tensao, 2);
  Serial.println(" V");
  
  Serial.print("[Corrente] ");
  Serial.print(corrente, 2);
  Serial.print(" mA");

  Serial.print("[Nivel] ");
  Serial.print(nivel, 1);
  Serial.println(" %"); 
}

//Enviar para API
void enviarParaAPI(
  float tensao,
  float corrente,
  float nivel
) {
  if (client.connect(server, 4000)) {
    Serial.print("Conectado na API");

    String json = "{";
    json += "\"device_id\":\"0001\",";
    json += "\"api_key\":\"chave-super-secreta\",";
    json += "\"tensao\":\"" + String(tensao, 2) + "\",";
    json += "\"corrente_ma\":\"" + String(corrente, 2) + "\",";
    json += "\"nivel_percentual\":\"" + String(nivel, 1) + "\"";
    json += "}";

    client.println("POST /api/readings HTTP/1.1");
    client.println("Host: 192.168.0.200");
    client.println("Content-Type: application/json");
    client.println("Connectio: close");

    client.print("Content-Length: ");
    client.println(json.length());

    client.println();
    client.println(json);

    Serial.println("JSON enviado: ");
    Serial.println(json);

    delay(10000);

    while (client.available()) {

      char c = client.read();

      Serial.write(c);
    }

    client.stop();

    Serial.println("Conexão encerrada");

  } else {
    Serial.println("Falha ao conectar API");
  }

}

void setup() {

  Serial.begin(9600);
  
  Serial.println(" SISTEMA DE TELEMETRIA INICIADO");

  Ethernet.begin(mac);

  delay(1000);

  Serial.print("IP Arduino: ");
  Serial.println(Ethernet.localIP()); 

}

void loop() {

  unsigned long agora = millis();
  
  if(agora - ultimaLeitura >= INTERVALO_LEITURA) {
    
    ultimaLeitura = agora;
    
    //leitura adc
    int adc = analogRead(SENSOR_PIN);
    
    //leitura filtrada
    float tensao = lerTensaoFiltrada();

    float corrente = calcularCorrente(tensao);
    
    float nivel = calcularNivel(tensao);
    
    exibirLeitura(
      adc, 
      tensao,
      corrente, 
      nivel
    );

    enviarParaAPI(
      tensao,
      corrente,
      nivel
    );

  }
}





