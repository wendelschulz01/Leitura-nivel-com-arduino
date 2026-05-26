const int SENSOR_PIN = A0;

//Faixa do sensor real
const float TENSAO_MIN = 1.0;
const float TENSAO_MAX = 5.0;

//Tempo entre leituras
const unsigned long INTERVALO_LEITURA = 500;

unsigned long ultimaLeitura = 0;

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

//logs
void exibirLeitura(int adc, float tensao, float nivel) {

  Serial.print("[ADC] ");
  Serial.println(adc);
  
  Serial.print("[Tensao] ");
  Serial.print(tensao, 2);
  Serial.println(" V");
  
  Serial.print("[Nivel] ");
  Serial.print(nivel, 1);
  
  Serial.println(" %"); 
}

void setup() {

  Serial.begin(9600);
  
  Serial.println(" SISTEMA DE TELEMETRIA INICIADO");

}

void loop() {

  unsigned long agora = millis();
  
  if(agora - ultimaLeitura >= INTERVALO_LEITURA) {
    
    ultimaLeitura = agora;
    
    //leitura adc
    int adc = analogRead(SENSOR_PIN);
    
    //leitura filtrada
    float tensao = lerTensaoFiltrada();
    
    float nivel = calcularNivel(tensao);
    
    exibirLeitura(adc, tensao, nivel);
  }
}