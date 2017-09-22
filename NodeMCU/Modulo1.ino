#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
 
// WIFI
const char* SSID = " "; // SSID da Rede
const char* PASSWORD = " "; // Senha da Rede
 
// MQTT
const char* BROKER_MQTT = "iot.eclipse.org"; // IP/URL DO BROKER MQTT
int BROKER_PORT = 1883; // Porta do Broker MQTT

//defines gerais 
#define ID_MQTT  "HomeAut"     //id mqtt (para identificação de sessão)
#define ENDERECO_MODULO      1         //endereço deste módulo
#define ENDERECO_MODULO_STR  "001"     //endereço deste módulo
#define TOPICO_SUBSCRIBE "MQTTHomeIOTEnvia"     //tópico MQTT de escuta
#define TOPICO_PUBLISH   "MQTTHomeIOTRecebe"    //tópico MQTT de envio de informações para Broker

//mapeamento de pinos do NodeMCU
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
char EstadoSaida1;
char EstadoSaida2;
char EstadoSaida3;
char EstadoSaida4;
 
//Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void EnviaEstadoOutputsMQTT(void);
void InitOutputs(void);
void AtuaSaida(String saida, String acao); 
/* 
 *  Implementações das funções
 */
void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  InitOutputs();
  initSerial();
  initWiFi();
  initMQTT();
}
 
//Função: inicializa comunicação serial
//Parâmetros: nenhum
//Retorno: nenhum
void initSerial() {
  Serial.begin(115200);
}

//Função: inicializa e conecta-se na rede WiFi desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi() 
{
    delay(10);
    Serial.println();
    Serial.print("Conectando-se em: ");
    Serial.println(SSID);
 
    WiFi.begin(SSID, PASSWORD); // Conecta na Rede Wireless
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
  
    Serial.println();
    Serial.print("Conectado na Rede ");
    Serial.print(SSID);
    Serial.println(" | IP ");
    Serial.println(WiFi.localIP());
}
 
//Função: inicializa parâmetros de conexão MQTT(endereço do broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
    MQTT.setCallback(mqtt_callback); //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
 
//Função: função de callback (chamada toda vez que uma informação de um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
    
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
  
    //faz o parse da string
    String Modulo = msg.substring(0, 3);
    String Saida  = msg.substring(4, 6);
    String Acao = msg.substring(7, 9);

    if (Modulo.equals(ENDERECO_MODULO_STR))
       AtuaSaida(Saida,Acao);
   
}

//Função: atua nas saidas
//Parâmetros: saida que deseja atiuar e ação (ligar ou desligar)
//Retorno: nenhum
void AtuaSaida(String saida, String acao)
{
    if (saida.equals("01"))
    {
        if (acao.equals("L"))            
        {
            digitalWrite(D0, HIGH);
            EstadoSaida1 = '1';
        }
        
        if (acao.equals("D"))            
        {
            digitalWrite(D0, LOW);   
            EstadoSaida1 = '0';
        }
        
        return; 
    }

    if (saida.equals("02"))
    {
        if (acao.equals("L"))            
        {
            digitalWrite(D1, HIGH);
            EstadoSaida2 = '1';
        }
        
        if (acao.equals("D"))            
        {
            digitalWrite(D1, LOW);   
            EstadoSaida2 = '0';
        }
        return; 
    }

    if (saida.equals("03"))
    {
        if (acao.equals("L"))
        {            
            digitalWrite(D2, HIGH);
            EstadoSaida3 = '1';
        }
        if (acao.equals("D"))  
        {          
            digitalWrite(D2, LOW);   
            EstadoSaida3 = '0';
        }
        
        return; 
    }

    if (saida.equals("04"))
    {
        if (acao.equals("L"))
        {            
            digitalWrite(D3, HIGH);
            EstadoSaida4 = '1';
        }
        
        if (acao.equals("D"))            
        {
            digitalWrite(D3, LOW);   
            EstadoSaida4 = '0';
        }
        
        return; 
    }
}
 
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else 
        {
            Serial.println("Falha ao Reconectar");
            Serial.println("Tentando se reconectar em 2 segundos");
            delay(2000);
        }
    }
}
 
//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void recconectWiFi() 
{
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
}

//Função: verifica o estado das conexões WiFI e ao broker MQTT. Em caso de desconexão (qualquer uma das duas), a conexão é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    
     recconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

//Função: envia ao Broker o estado atual de todos os outputs 
//Parâmetros: nenhum
//Retorno: nenhum
void EnviaEstadoOutputsMQTT(void)
{
    char payloadChar[13];
    int EnderecoModulo = ENDERECO_MODULO;
    char payload[11];

    sprintf(payloadChar,"%03d>%c>%c>%c>%c", EnderecoModulo,
                                            EstadoSaida1,
                                            EstadoSaida2,
                                            EstadoSaida3,
                                            EstadoSaida4);
    memcpy(payload,payloadChar,11);

    Serial.println("- Estado das saidas enviado ao broker!");
    
    MQTT.publish(TOPICO_PUBLISH, payloadChar);
    delay(1000);
}

//Função: inicializa todos os outputs em nível lógico baixo
//Parâmetros: nenhum
//Retorno: nenhum
void InitOutputs(void)
{
    digitalWrite(D0, LOW);          
    digitalWrite(D1, LOW);          
    digitalWrite(D2, LOW);          
    digitalWrite(D3, LOW);           
    
    EstadoSaida1 = '0';
    EstadoSaida2 = '0';
    EstadoSaida3 = '0';
    EstadoSaida4 = '0'; 
}
//programa principal
void loop() 
{
    
    
    //garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();

    //envia o status de todos os outputs para o Broker no protocolo esperado
    EnviaEstadoOutputsMQTT();

    //keep-alive da comunicação com broker MQTT
    MQTT.loop();
}
