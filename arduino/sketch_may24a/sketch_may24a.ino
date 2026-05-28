#include <SPI.h>
#include <Ethernet.h>

const int SENSOR_PIN = A0;


const float TENSAO_MIN = 1.0;
const float TENSAO_MAX = 5.0;

//Faixa física da sonda
const float NIVEL_MAXIMO_METROS = 10.0;

//Offset de calibração
const float OFFSET_METROS = 0.0;


//Leitura do sensor
const unsigned long INTERVALO_LEITURA = 500;

//Envio para API
const unsigned long INTERVALO_ENVIO = 10000;


unsigned long ultimaLeitura = 0;
unsigned long ultimoEnvio = 0;

// VARIÁVEIS GLOBAIS
float tensaoAtual = 0;
float nivelPercentualAtual = 0;
float nivelMetrosAtual = 0;


//MAC da shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

//IP do servidor(PC por enquanto)
IPAddress server(192, 168, 0, 200);

EthernetClient client;


float lerTensaoFiltrada() {

  float soma = 0;

  //média de 10 leituras
  for (int i = 0; i < 10; i++) {

    int adc = analogRead(SENSOR_PIN);

    float tensao =
      adc * (5.0 / 1023.0);

    soma += tensao;

    delay(5);
  }

  return soma / 10.0;
}


float calcularNivelPercentual(
  float tensao
) {

  //clamp
  if (tensao < TENSAO_MIN)
    tensao = TENSAO_MIN;

  if (tensao > TENSAO_MAX)
    tensao = TENSAO_MAX;

  float nivel =
    ((tensao - TENSAO_MIN) /
    (TENSAO_MAX - TENSAO_MIN))
    * 100.0;

  return nivel;
}


float calcularNivelMetros(
  float tensao
) {

  //clamp
  if (tensao < TENSAO_MIN)
    tensao = TENSAO_MIN;

  if (tensao > TENSAO_MAX)
    tensao = TENSAO_MAX;

  //conversão linear
  float nivel =
    ((tensao - TENSAO_MIN) /
    (TENSAO_MAX - TENSAO_MIN))
    * NIVEL_MAXIMO_METROS;

  //offset
  nivel += OFFSET_METROS;

  //evitar negativo
  if (nivel < 0)
    nivel = 0;

  return nivel;
}


//logs
void exibirLeitura(
  int adc,
  float tensao,
  float nivelPercentual,
  float nivelMetros
) {

  Serial.println();

  Serial.print("[ADC] ");
  Serial.println(adc);

  Serial.print("[Tensao] ");
  Serial.print(tensao, 2);
  Serial.println(" V");

  Serial.print("[Nivel %] ");
  Serial.print(nivelPercentual, 1);
  Serial.println(" %");

  Serial.print("[Nivel m] ");
  Serial.print(nivelMetros, 2);
  Serial.println(" m");

  Serial.println("-------------------");
}


//enviar para API
void enviarParaAPI(
  float tensao,
  float nivelPercentual,
  float nivelMetros
) {

  if (client.connect(server, 4000)) {

    Serial.println("Conectado na API");

    //JSON
    String json = "{";

    json += "\"device_id\":\"0001\",";
    json += "\"api_key\":\"chave-super-secreta\",";
    json += "\"tensao\":" + String(tensao, 2) + ",";
    json += "\"nivel_percentual\":" + String(nivelPercentual, 1) + ",";
    json += "\"nivel_metros\":" + String(nivelMetros, 2);

    json += "}";

    //HTTP
    client.println("POST /api/readings HTTP/1.1");
    client.println("Host: 192.168.0.200");
    client.println("Content-Type: application/json");
    client.println("Connection: close");

    client.print("Content-Length: ");
    client.println(json.length());

    client.println();

    //BODY
    client.println(json);

    Serial.println("JSON enviado:");
    Serial.println(json);

    //timeout 
    unsigned long timeout = millis();

    while (
      client.connected() &&
      millis() - timeout < 5000
    ) {

      while (client.available()) {

        char c = client.read();

        Serial.write(c);

        timeout = millis();
      }
    }

    client.stop();

    Serial.println();
    Serial.println("Conexao encerrada");
  }
  else {

    Serial.println("Falha ao conectar API");
  }
}


void setup() {

  Serial.begin(9600);

  Serial.println();
  Serial.println("SISTEMA DE TELEMETRIA INICIADO");

  //DHCP
  if (Ethernet.begin(mac) == 0) {

    Serial.println(
      "Falha ao obter IP via DHCP"
    );

    while (true);
  }

  delay(1000);

  Serial.print("IP Arduino: ");
  Serial.println(Ethernet.localIP());
}


void loop() {

  unsigned long agora = millis();

  
  if (
    agora - ultimaLeitura >=
    INTERVALO_LEITURA
  ) {

    ultimaLeitura = agora;

    //ADC bruto
    int adc =
      analogRead(SENSOR_PIN);

    //tensão filtrada
    tensaoAtual =
      lerTensaoFiltrada();

    //nível %
    nivelPercentualAtual =
      calcularNivelPercentual(
        tensaoAtual
      );

    //nível metros
    nivelMetrosAtual =
      calcularNivelMetros(
        tensaoAtual
      );

    //logs
    exibirLeitura(
      adc,
      tensaoAtual,
      nivelPercentualAtual,
      nivelMetrosAtual
    );
  }

//enviar para API
  if (
    agora - ultimoEnvio >=
    INTERVALO_ENVIO
  ) {

    ultimoEnvio = agora;

    enviarParaAPI(
      tensaoAtual,
      nivelPercentualAtual,
      nivelMetrosAtual
    );
  }
}