# Leitura de Nível com Arduino
Projeto de leitura e transmissão de dados de sonda de nível com arduíno.

## Requisitos iniciais:

* Alimentar sonda de nível com tensão de corrente contínua de 8 a 32 V;
* Coletar dados da sonda em portas analógicas. O sensor tem saida com amperagem de 4 a 20 mAh;
* O sensor tem uma faixa de leitura de 0 a 10 MCA;
* Transmitir leituras via internet até o servidor;
* Gravar os dados no banco;




Primeira simulação utilizando potenciômetro:
<img width="583" height="347" alt="image" src="https://github.com/user-attachments/assets/5110d137-f484-472b-8a82-2df124af566d" />



### Detalhes da instalação real:

* Sensor é alimentado com 24V (fio vermelho) e o negativo (fio preto) é ligado na porta analógica.
* Para obter uma litura precisa, é necessário um valor de correção. O motivo é que o sensor nunca vai estar até o fundo do reservatório analisar, mas precisa ler esse "espaço" que falta.


