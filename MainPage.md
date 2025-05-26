\mainpage Documentação Mini Projeto SETR 24/25

## Introdução 
Com este curto projeto pretendemos desenvolver um protótipo de um sistema de controlo de um processo térmico. O atuador é implementaddo com uma resistência de potência e a sensibilidade à temperatura é feita através d um sensor digital de temperatura (interface I2C).

O sistema tem duas interfaces de operação, uma para configuração e outra para operaçãos normal:
* A interface de configuração é feita através de UART. Um engenheiro pode usar um terminal do PC para definir alguns parâmetros relevantes, como a máxima temperatura aceitável e as definições do controlador, e também obter dados em tempo-real sobre o processo;
* A interface de operação permite o utilizador ligar/desligar o sistema e definir a temperatura pretendida. Há também alguns indicadores em relação ao estado de operação do sistema.

## Especificações 
**Interface de Operação:**
* Button 1: Liga/Desliga o sistema;
* Button 2: Aumenta a temperatura pertendida em 1ºC;
* Button 3: Diminui a temperatura pertendida em 1ºC;
* Led 1: indica se o sistema está ligado (led on) ou se está desligado (led off);
* Led 2: "normal state", indica que o sistema está no estado normal, isto é, a temperatura está dentro de uma gama de +/- 2ºC da temperatura pertendida;
* Led 3: "warning-low", indica que o sistema está abaixo do pretendido, isto é, a temperatura está 2ºC abaixo da temperatura pertendida;
* Led 4: "warning-high", indica que o sistema está acima do pretendido, isto é, a temperatura está 2ºC acima da temperatura pertendida;

**Interface de Configuração:**
* via UART, set to 115200, 8, n, 1;
* Orientação em frame. Todas as comunicações são em ASCII, e. g. se a temperatura for +25ºC, os bytes correspondentes serão 43 ('+'), 50 ('2') e 53 ('5');

Estrutura dos comandos:  **#CMD DATA CS!** (sem espaços entre os campos)
* **- #**: um byte, símbolo de inicio do frame
* **- !**: um byte, símbolo de fim do frame
* **- CMD**: um byte, define o comando/resposta
* **- DATA**: tamanho variável, argumentos do comando/resposta
* **- CS**: modulo-256 checksum. Somatório do valor numérico de CMD e DATA[i] bytes

Comandos suportados:
* **- #Mxxxyyy!**: define o valor máximo da temperatura em "xxx"(ºC). "yyy" correspondente ao checksum;
* **- #Cyyy!**: obtem a temperatura atual do processo. "yyy" correspondente ao checksum;
* **- #Sxx...xxyyy!**: define os parâmetros do controlador. "xx...xx" são os parâmetros *(Kp, Ti, Td para PID/histerese para on/off controlador (apagar!!!!!!!!!))*. "yyy" correspondente ao checksum;
* **- #_________!**: 

Comandos de confirmação:
* **-#Eoyyy!**: Sem erros/Ok. "yyy" correspondente ao checksum;
* **-#Efyyy!**: Erros de frames. "yyy" correspondente ao checksum;
* **-#Esyyy!**: Erro checksum. "yyy" correspondente ao checksum;
* **-#Eiyyy!**: Comando inválido. "yyy" correspondente ao checksum;

* Comandos em que há uma resposta explícita (E.g. #Cyyy!) não precisão de confirmação. As respostas a estes comandos é uma confirmação implícita de sucesso, nestes casos. 

\author Guilherme Santos, gui.santos@ua.pt
\author Francisco Bastos, fcbastos4@ua.pt
