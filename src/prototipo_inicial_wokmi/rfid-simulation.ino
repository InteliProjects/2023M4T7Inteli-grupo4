//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>
 
//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(5, 18, 4, 2, 19, 21);

int redLed = 16;
int greenLed = 17;
int sucessButton = 12;
int failedButton = 25;
// Definindo o tamanho máximo da lista de nomes
const int tamanhoMaximo = 5;
 // Criando um array de strings para armazenar os nomes
std::string nomes[tamanhoMaximo] = {"Alice", "Bob", "Charlie", "David", "Eva"};
std::string salas[tamanhoMaximo] = {"Sala 1", "Sala 2", "Sala 3", "Sala 4", "Sala 5"};
int indiceAleatorio;


void setup()
{
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);
  pinMode(sucessButton,INPUT);
  pinMode(failedButton,INPUT);
  pinMode(redLed,OUTPUT);
  pinMode(greenLed,OUTPUT);
  initialMessage();
}
 
void loop()
{
  if(digitalRead(sucessButton) == 1)
  {
    lcd.clear();
    // Gerando um índice aleatório
    indiceAleatorio = rand() % tamanhoMaximo;
    // lcd.print("Cartao validado");
    lcd.setCursor(0, 0);
    lcd.print(nomes[indiceAleatorio].c_str()); // Convertendo para const char*
    lcd.setCursor(0,1);
    lcd.print(salas[indiceAleatorio].c_str()); // Convertendo para const char*
    digitalWrite(greenLed,HIGH);
    delay(3000);
    initialMessage();
    digitalWrite(greenLed,LOW);
  }

  else if(digitalRead(failedButton) == 1)
  {
    lcd.clear();
    digitalWrite(redLed,HIGH);
    lcd.print("Cartao Invalido");
    delay(1000);
    initialMessage();
    digitalWrite(redLed,LOW);
  }
}

void initialMessage()
{
  lcd.clear();
  lcd.print(" Aproxime o seu");  
  lcd.setCursor(0,1);
  lcd.print("cartao do leitor");  
} 