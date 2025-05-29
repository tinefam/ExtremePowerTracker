/*
Created by Francesco Tinelli
Versione 1.20 (last updated on 29.05.2025)
*/ 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

void pinMode();
void modoStandard();
void caricamento();
void motore();
void sottomenufuz();
void menu();
void posizioneCursore();
void menuVisualizzato();
void modoMenu();
void blocco();

// Pin
#define pinLength 5  // Password lunga 4 caratteri 0000
char pinIn[pinLength];
char pin[pinLength] = "2025"; // Modifica Password
char dataIn;
byte dataCount = 0;
bool accesso = true; // Mettilo a true per passare bypassare l'accesso
int funzioni0 = 0;
int funzioni1 = 0;
int funzioni2 = 0;
int funzioni4 = 0;
char key2;
char key3;
char key4;

// Motore
bool motoreAttivo = true; // Mettilo a falso per disattivare i motori
int caricamentoIniziale = 0; // Mettilo a 1 per rimuovere i tempi di attesa
Servo motoreOrizzontale;
Servo motoreVerticale;
int servoH = 90;
int servoV = 90;
#define pinSwitchFoto 12 // Pin di controllo switch CD4066 fotocellula
#define pinSwitchPannello 13 // Pin di controllo switch CD4066 servi
#define fotoLedHighLeft 0 // Pin ananalogico fotocellula in alto a sinistra
#define fotoLedHighRight 1 // Pin ananalogico fotocellula alto a destra
#define fotoLedLowLeft 2// Pin ananalogico fotocellula in basso a sinistra
#define fotoLedLowRight 3 // Pin ananalogico fotocellula in basso a destra
#define pinPannello 4 // Pin panello solare
#define pinMotoreOrizzontale 10 // Pin motore orizzontale
#define pinMotoreVerticale 11 // Pin motore orizzontale 
#define tempoUpdate 10 // Tempo di ritardo prima di un aggiornamento
#define tol 50 // Tolleranza aggiornamento
#define tolPosizManuale 15

//Pannello
int valAssorbimento;
int valAssorbimentoPerc;

// Menu
int posizioneAuto = 0;
int posizioneManu = 0;
int monitorAttivo = 0;
#define minFunzione 0
#define maxFunzione 4
char stringhe[5][15] = {
  "Pannelli",
  "Fotocellule",
  "Posizione",
  "Batteria",
  "Blocco",
};
int numeroFunzione = 0;
int pagina = 0;
int exPagina = 1;
int paginaSottoMenu = 0;
int exPaginaSottoMenu = 1;
int sottomenu = 0;

// Setup tastierino
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8, 9 };

/* Dichiarazione tastierino = Keypad(makeKeymap(matrice tastierino), pin row (i primi 4 da sinistra) in array, 
  pin col (i primi 4 da destra) in array, num righe, num colonne);*/
Keypad tastierino = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);

char key;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.setBacklight(HIGH);
  if(motoreAttivo==true)
  {
    motoreOrizzontale.attach(pinMotoreOrizzontale); // Setup pin motore orizzontale
    motoreVerticale.attach(pinMotoreVerticale); // Setup pin motore verticale
  }
  pinMode(pinSwitchFoto, OUTPUT);
  pinMode(pinSwitchPannello, OUTPUT);
}

void loop() {
    key = tastierino.getKey();

    if (!accesso) {
        pinMode();
    }
    else 
    {
        if (caricamentoIniziale == 0) {
            caricamento();
            caricamentoIniziale = 1;
        }
        modoStandard();
    }
}


void pinMode() // Fase pin
{
    lcd.setCursor(0, 0);
    lcd.print("Pin:");

    // Valore dal pulsante
    dataIn = tastierino.getKey(); 
    if (dataIn) {
      pinIn[dataCount] = dataIn; 
      lcd.setCursor(dataCount, 1); 
      lcd.print('*'); // Censura Pin
      dataCount++; // Conta le lettere
    }

    if(dataCount == pinLength-1) // Confronto tra pin immesso e il pin
    {
      lcd.setCursor(0, 0);
      delay(700);
      lcd.clear();
      delay(850);

      if(!strcmp(pinIn, pin))
        // Pin corretto
        {
          lcd.setCursor(0, 0);
          lcd.print("Pin corretto,");
          lcd.setCursor(0, 1);
          lcd.print("benvenuto!");
          accesso = true;
          monitorAttivo = 0; // Assicura che monitorAttivo sia 0 per iniziare
          numeroFunzione = 0; // Reset numeroFunzione per il menu principale
          sottomenu = 0; // Assicura che sottomenu sia 0
        }
        else
        // Pin sbagliato
        {
          lcd.print("Pin sbagliato!");
        }

      // Pulisci schermo
      delay(3000);
      lcd.clear();
      dataCount = 0;
    }
}

  void caricamento() // Solo all'inzio, prima del mondo standard, fa un finto caricamento
  {
    lcd.setCursor(0, 0);
    lcd.print("Caricamento");
    lcd.setCursor(11, 0);
    lcd.print(".");
    delay(1500);
    lcd.setCursor(12, 0);
    lcd.print(".");
    delay(1500);
    lcd.setCursor(13, 0);
    lcd.print(".");
    delay(1500);
    lcd.clear();
  }

  void modoStandard()
{
    if((key=='0') && (monitorAttivo == 1))
    {
      monitorAttivo--;
      lcd.clear();
    }
    else if((key=='0') && (monitorAttivo == 0))
    {
      monitorAttivo++;
      exPagina = 1;
      sottomenu = 0;
      numeroFunzione = 0;
      funzioni2 = 0;
      lcd.clear();
    }

    if(monitorAttivo == 0)
    {
      motore();
    }
    else if(monitorAttivo == 1)
    {
      if(funzioni2 == 1) {
        posizionePredefinita();
      } else {
        modoMenu();
      }
    }
}


void modoMenu()
{
  if((key == '1') && (paginaSottoMenu == 2))
  {
  funzioni2 = 1;
  }
  else if((key == '2') && (paginaSottoMenu == 2))
  {
  funzioni2 = 2;
  }

  if((key == '1') && (paginaSottoMenu == 4))
  {
  funzioni4 = 1;
  }
  else if((key == '2') && (paginaSottoMenu == 2))
  {
  funzioni4 = 2;
  }

  if((key=='A') && (numeroFunzione > minFunzione) && (sottomenu == 0))
  {
    numeroFunzione--;
  }
  else if((key=='B') && (numeroFunzione < maxFunzione) && (sottomenu == 0)) 
  {
    numeroFunzione++;
  }
  
  if((key=='#') && (sottomenu == 0))
  {
    exPaginaSottoMenu = 1;
    sottomenu++;
    funzioni1 = 0;
  }
  else if((key=='*') && (sottomenu == 1))
  {
    exPaginaSottoMenu = 0;
    lcd.clear();
    sottomenu--;
    funzioni0 = 0;
    funzioni2 = 0;
    funzioni4 = 0;
    exPagina = 3;
  }

  if(sottomenu == 0)
  {
    menu();
  }
  else if(sottomenu == 1)
  {
    sottomenufuz();
  }
}

void motore()
{
  lcd.setCursor(0, 0);
  lcd.print("Xtreme Power");
  lcd.setCursor(0, 1);
  lcd.print("Tracker Ver.1.20");

  digitalWrite(pinSwitchFoto, HIGH);
  digitalWrite(pinSwitchPannello, LOW);
  int leftHigh = analogRead(fotoLedHighLeft); // Prende valore led in alto a sinistra
  int rightHigh = analogRead(fotoLedHighRight); // Prende valore led in alto a destra
  int leftLow = analogRead(fotoLedLowLeft); // Prende valore led in basso a sinistra
  int rightLow = analogRead(fotoLedLowRight); // Prende valore led in basso a destra

  // Trascrizione valori delle fotocellule sul seriale
  Serial.print("LeftHigh = ");
  Serial.print(leftHigh);
  Serial.print(" RightHigh = ");
  Serial.print(rightHigh);
  Serial.print(" LeftLow = ");
  Serial.print(leftLow);
  Serial.print(" RightLow = ");
  Serial.println(rightLow);

  int mediaHigh = (leftHigh + rightHigh) / 2; // Media valori fotocellule in alto
  int mediaLow = (leftLow + rightLow) / 2; // Media valori fotocellule in basso
  int mediaLeft = (leftHigh + leftLow) / 2; // Media valori fotocellule a sinistra
  int mediaRight = (rightHigh + rightLow) / 2; // Media valori fotocellule a destra

  int difVertical = mediaHigh - mediaLow; // Differenza tra valore superiore e valore inferiore
  int difHorizontal = mediaLeft - mediaRight; // Differenza tra valore sinistro e valore destro

  
  if ((difVertical <= -tol) || (difVertical >= tol))
  {
    if (mediaHigh > mediaLow) // Confrontiamo valore superiore con valore inferiore
    {
      servoV++; // Gira verso il basso
      if (servoV > 180) // Non supera 180
      { 
        servoV = 180;
      }
    }
    else if (mediaLeft < mediaRight) // Confrontiamo valore sinistro con destro
    {
      servoV--; // Gira verso l'alto
      if (servoV < 0) // Non diventa negativo
      { 
        servoV = 0;
      }
    }
    motoreVerticale.write(servoV);
  }

  if ((difHorizontal <= -tol) || (difHorizontal >= tol))
  {
    if (mediaHigh > mediaLow) // Confrontiamo valore superiore con valore inferiore
    {
      servoH++; // Gira verso destra
      if (servoH > 180) // Non supera 180
      { 
        servoH = 180;
      }
    }
    else if (mediaLeft < mediaRight) // Confrontiamo valore sinistro con destro
    {
      servoH--; // Gira verso sinistra
      if (servoH < 0) 
      { // Non diventa negativo
        servoH = 0;
      }
    }
    motoreOrizzontale.write(servoH);
  }
  delay(tempoUpdate); // Tempo di ritardo per aggiornare
}

void menu(){
  posizioneCursore();
}

void posizioneCursore(){
  if(numeroFunzione % 2 == 0) {
    lcd.setCursor(0, 0);
    lcd.print(">");
    lcd.setCursor(0, 1);
    lcd.print(" ");
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print(">");
  }

  menuVisualizzato();
}

void menuVisualizzato(){
  
    switch(numeroFunzione)
    {
      case 0:
      case 1:
      {
        pagina = 0;
        break;
      }
      case 2:
      case 3:
      {
        pagina = 1;
        break;
      }
      case 4:
      {
        pagina = 2;
        break;
      }
      default:
      {
        break;
      }
    }
  
    if(pagina != exPagina)
    {
      switch(pagina)
      {
        case 0:
        {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print(stringhe[0]);
          lcd.setCursor(2, 1);
          lcd.print(stringhe[1]);
          break;
        }
        case 1:
        {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print(stringhe[2]);
          lcd.setCursor(2, 1);
          lcd.print(stringhe[3]);
          break;
        }
        case 2:
        {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print(stringhe[4]);
          break;
        }
        default:
        {
          lcd.setCursor(2, 0);
          lcd.print("404");
        }
      }
      exPagina = pagina;
    }
}


void sottomenufuz() {
  // Seleziona la pagina corretta in base alla funzione principale
  switch (numeroFunzione) {
      case 0:
          paginaSottoMenu = 0; // Opzioni del sottomenu per la funzione 0
          break;
      case 1:
          paginaSottoMenu = 1; // Opzioni del sottomenu per la funzione 1
          break;
      case 2:
          paginaSottoMenu = 2; // Opzioni del sottomenu per la funzione 2
          break;
      case 3:
          paginaSottoMenu = 3; // Opzioni del sottomenu per la funzione 3
          break;
      case 4:
          paginaSottoMenu = 4; // Opzioni del sottomenu per la funzione 4
          break;
      default:
          break;
  }

  // Mostra il sottomenu corretto in base alla paginaSottoMenu
  switch (paginaSottoMenu) {
      case 0:
          pannelli();
          break;
      case 1:
          fotoresistenze();
          break;
      case 2:
          posizione();
          break;
      case 3:
          Batteria();
          break;
      case 4:
          Blocco();
          break;
      default:
          break;
  }
}



void pannelli() // Menu pannelli
{
  if(funzioni0 == 0)
{
  lcd.clear();
  digitalWrite(pinSwitchFoto, LOW);
  digitalWrite(pinSwitchPannello, HIGH);

  valAssorbimento = analogRead(pinPannello);
  valAssorbimentoPerc = (valAssorbimento*100)/1023;

  lcd.setCursor(0, 0);
  lcd.print("Assorbimento");
  lcd.setCursor(0, 1);
  lcd.print("Pannelli  ");
  lcd.setCursor(11, 1);
  lcd.print(abs(valAssorbimentoPerc));
  lcd.setCursor(14, 1);
  lcd.print("%");
  funzioni0 = 1;
}


    if ((key == '*') && (key == '#')) {
      funzioni0 = 0;
      exPagina = 4;
    }
}

void fotoresistenze() {
 if(funzioni1 == 0)
  {
    lcd.clear();
    digitalWrite(pinSwitchFoto, HIGH);
    digitalWrite(pinSwitchPannello, LOW);

    int valAssorbimentoLH = analogRead(fotoLedHighLeft); // Prende valore led in alto a sinistra
    int valAssorbimentoRH = analogRead(fotoLedHighRight); // Prende valore led in alto a destra
    int valAssorbimentoLL = analogRead(fotoLedLowLeft); // Prende valore led in basso a sinistra
    int valAssorbimentoRL = analogRead(fotoLedLowRight); // Prende valore led in basso a destra

    int valAssorbimentoPercLH = (valAssorbimentoLH*100)/1023;
    int valAssorbimentoPercRH = (valAssorbimentoRH*100)/1023;
    int valAssorbimentoPercLL = (valAssorbimentoLL*100)/1023;
    int valAssorbimentoPercRL = (valAssorbimentoRL*100)/1023;


    lcd.setCursor(0, 0);
    lcd.print("Luce fotocellule");
    lcd.setCursor(0, 1);
    lcd.print("LH:   %|");
    lcd.setCursor(4, 1);
    lcd.print(abs(valAssorbimentoPercLH));
    lcd.setCursor(8, 1);
    lcd.print("RH:   %|");
    lcd.setCursor(12, 1);
    lcd.print(abs(valAssorbimentoPercRH));
    lcd.setCursor(16, 1);
    lcd.print("LL:   %|");
    lcd.setCursor(20, 1);
    lcd.print(abs(valAssorbimentoPercLL));
    lcd.setCursor(24, 1);
    lcd.print("RL:   %|");
    lcd.setCursor(28, 1);
    lcd.print(abs(valAssorbimentoPercLH));

    delay(2000);
    for (int i = 0; i < 16; i++) { 
         lcd.scrollDisplayLeft(); //il testo scorrerà verso sinistra
         delay(900); 
    } 
    funzioni1 = 1;
  }
    exPagina = 4;

}


void posizione() {
    // Leggi il tasto premuto
    key2 = tastierino.getKey();

    // Se funzioni2 è 0, mostra il menu di selezione
    if (funzioni2 == 0) {
        lcd.setCursor(0, 0);
        lcd.print("1. Posiz. auto ");
        lcd.setCursor(0, 1);
        lcd.print("2. Posiz. manu ");

        // Controlla se viene premuto un tasto per aggiornare funzioni2
        if (key2 == '1') {
            funzioni2 = 1;
            lcd.clear(); // Pulisci lo schermo per la nuova visualizzazione
        } else if (key2 == '2') {
            funzioni2 = 2;
            lcd.clear(); // Pulisci lo schermo per la nuova visualizzazione
        }
    }

    // Esegui la funzione selezionata
    if(key)
    {
      lcd.clear();
       switch (funzioni2) {
        case 1:
            posizionePredefinita();
            break;
        case 2:
            posizioneManuale();
            break;
        default:
            break;
    }
    }
   

    // Riporta funzioni2 a 0 quando la funzione selezionata è terminata
    if (funzioni2 != 0 && (key2 == '#' || key2 == '*')) {
        funzioni2 = 0;
        exPagina = 4;
        lcd.clear(); // Pulisci lo schermo per tornare al menu di selezione
    }
}

void posizionePredefinita() {
  lcd.setCursor(0, 0);
  lcd.print("Direz:");
  lcd.setCursor(0, 1);
  lcd.print("1.  2.  3.");

  // Leggi il tasto premuto
  key = tastierino.getKey();

  // Controlla quale tasto è stato premuto
  if (key == '1' || key == '2' || key == '3') {
    switch (key) {
      case '1': // Est
        servoH = 180; // Imposta la posizione orizzontale per est
        servoV = 90; // Imposta la posizione verticale di default
        break;
      case '2': // Sud
        servoH = 90; // Imposta la posizione orizzontale di default
        servoV = 0; // Imposta la posizione verticale per sud
        break;
      case '3': // Ovest
        servoH = 0; // Imposta la posizione orizzontale per ovest
        servoV = 90; // Imposta la posizione verticale di default
        break;
    }

    // Muovi il motore alla posizione desiderata
    motoreOrizzontale.write(servoH);
    motoreVerticale.write(servoV);

    // Visualizza un messaggio di conferma sulla LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Posizione");
    lcd.setCursor(0, 1);
    lcd.print("impostata!");
    delay(2000); // Mostra il messaggio per 2 secondi
    lcd.clear();
  }

  // Se viene premuto il tasto '*' (ritorno al menu principale), resetta la variabile di stato e torna al menu
  if (key == '*') {
    funzioni2 = 0; // Resetta la variabile di stato
    monitorAttivo = 1; // Torna al menu principale
    lcd.clear(); // Pulisci lo schermo
  }
}




void posizioneManuale() {
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Usa le freccie");
lcd.setCursor(0, 1);
lcd.print("per muovere:");

switch (key) {
        case '2': // Su
            servoV += tolPosizManuale;
            if (servoV > 180) {
                servoV = 180;
            }
            motoreVerticale.write(servoV);
            break;
        case '8': // Giù
            servoV -= tolPosizManuale;
            if (servoV < 0) {
                servoV = 0;
            }
            motoreVerticale.write(servoV);
            break;
        case '6': // Sinistra
            servoH -= tolPosizManuale;
            if (servoH < 0) {
                servoH = 0;
            }
            motoreOrizzontale.write(servoH);
            break;
        case '4': // Destra
            servoH += tolPosizManuale;
            if (servoH > 180) {
                servoH = 180;
            }
            motoreOrizzontale.write(servoH);
            break;
        default:
            break;
    }
    delay(tempoUpdate);
}



void Batteria()
{
  lcd.setCursor(0, 0);
  lcd.print("Batteria non");
  lcd.setCursor(0, 1);
  lcd.print("disponibile!");
}

void Blocco() {
    key4 = tastierino.getKey(); // Leggi il tasto premuto

    // Se funzioni4 è 0, mostra il menu di selezione
    if (funzioni4 == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Bloccare?");
        lcd.setCursor(0, 1);
        lcd.print("1.Si      2.No");
    }

    // Controlla se viene premuto un tasto per aggiornare funzioni4
    if (key == '1') {
        funzioni4 = 0;
        lcd.clear(); // Pulisci lo schermo per la nuova visualizzazione
        accesso = false; // Imposta accesso a falso per bloccare l'accesso
        numeroFunzione = 0; // Resetta la funzione selezionata
        sottomenu = 0; // Resetta il sottomenu
        monitorAttivo = 0;

        // Visualizza un messaggio di blocco
        lcd.setCursor(0, 0);
        lcd.print("Sistema");
        lcd.setCursor(0, 1);
        lcd.print("Bloccato!");
        delay(2500); // Attendi 2.5 secondi prima di pulire lo schermo
        lcd.clear();
        delay(500); // Attendi un mezzo secondo

        pinMode(); // Richiama la funzione pinMode per inserire il PIN
    } else if (key == '2') {
        // Se viene premuto il tasto '2', resettiamo il display e le variabili
        lcd.clear(); // Pulisci lo schermo
        funzioni4 = 0; // Reset funzioni4 per tornare al menu precedente
        numeroFunzione = 4; // Resetta la funzione selezionata
        sottomenu = 0; // Resetta il sottomenu
        monitorAttivo = 1;
        exPagina = 5; // Reset interfaccia
    }
}





