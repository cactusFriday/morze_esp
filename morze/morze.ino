#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "index.h"

// Connection credentials
const char* ssid = "?";
const char* password = "";

// Pin connected to the LED.
const int led = LED_BUILTIN;

// Global web server instance.
ESP8266WebServer server(80);

// Number of milliseconds for a dot in morse code.
const int morse_dot_ms = 100;
// Dash is 3 times the length of a dot.
const int morse_dash_ms = morse_dot_ms*3;
// Delay between parts of a character is the length of a dot.
const int morse_char_delay_ms = morse_dot_ms;
// Delay between letters in a word is the length three dots.
const int morse_letter_delay_ms = morse_dot_ms*3;
// Delay between words is seven dots.
const int morse_word_delay_ms = morse_dot_ms*7;

const int led_on  = HIGH;
const int led_off = LOW;

// Maximum length of message, used for internal buffer size.
const int max_message_len = 500;

// Table of character to morse code mapping.
// Each string represents the dashes and dots for a character.
const char* morse_codes_rus[31] = {
  ".-",     // А
  "-...",   // Б
  "--.",    // Г
  "-..",    // д
  ".",      // E
  "...-",   // Ж
  "--..",   // З
  "..",     // И
  ".---",   // Й
  "-.-",    // К
  ".-..",   // Л
  "--",     // М
  "-.",     // Н
  "---",    // О
  ".--.",   // П
  ".-.",    // Р
  "...",    // С
  "-",      // Т
  "..-",    // У
  "..-.",   // Ф
  "....",   // Х
  "-.-.",   // Ц
  "---.",   // Ч
  "----",   // Ш
  "--.-",   // Щ
  "-..-",   // Ъ
  "..-..",  // Э
  "..--",   // Ю
  ".-.-",   // Ю
};

const char* morse_codes[36] = {
  ".-",     // A
  "-...",   // B
  "-.-.",   // C
  "-..",    // D
  ".",      // E
  "..-.",   // F
  "--.",    // G
  "....",   // H
  "..",     // I
  ".---",   // J
  "-.-",    // K
  ".-..",   // L
  "--",     // M
  "-.",     // N
  "---",    // O
  ".--.",   // P
  "--.-",   // Q
  ".-.",    // R
  "...",    // S
  "-",      // T
  "..-",    // U
  "...-",   // V
  ".--",    // W
  "-..-",   // X
  "-.--",   // Y
  "--..",   // Z
  "----",   // 0
  ".----",  // 1
  "..---",  // 2
  "...--",  // 3
  "....-",  // 4
  ".....",  // 5
  "-....",  // 6
  "--...",  // 7
  "---..",  // 8
  "----."   // 9
};


void setup(void)
{
  Serial.begin(115200);
  
  // Setup LED as an output and turn it off.
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  // Connect to wifi AP.
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection to finish and print details.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Configure web server root handler to return the main 
  // HTML page. 
  server.on("/", []() {
    // Serve up the main HTML page.
    server.send(200, "text/html", index_html);
  });
  
  // Configure a handler for the /morse endpoint.
  server.on("/morse", []() {
    // Blink out the message in morse code, then respond 
    // with the root HTML page again.

    // Grab the message form parameter and fail if it's
    // larger than our max allowed size.
    const char* message = server.arg("message").c_str();
    if (strlen(message) >= max_message_len) {
      server.send(500, "text/plain", "Message is too long!");
      return;
    }
    // Perform form URL decoding to get the plain message.
    char decoded[max_message_len] = {0};
    form_url_decode(message, decoded);
    // Print and blink the message!
    Serial.print("Blinking message: "); Serial.println(decoded);
    blink_morse(led, decoded);
    // Return the main page again.
    server.send(200, "text/html", index_html);
  });
  
  server.begin();
  Serial.println("Morse code HTTP server started!");
}
 
void loop(void)
{
  // Take care of handling any web server requests.
  server.handleClient();
}

void blink_morse_char(int led_pin, char c) {
  // Blink out the morse code for the specified character.
  // Unrecognized characters are ignored.

  // First look up the string of morse code for the character.
  const char* code = NULL;
  int idx = 0;
  if ((c >= 'A') && (c <= 'Z')) {
    // Get the code for an alphabet character.
    idx = c - 'A';
  }
  else if ((c >= '0') && (c <= '9')) {
    // Get the code for a number.
    idx = c - '0' + 26;
  }
  // В случае русской буквы, она записывается в 2 элемента массива и 
  // всегда начинается с 208. 
  // else if (int(c) == 208) {
  //   code = morse_codes[int(c) - 191];
  // }
  else {
    // Unknown character
    return;
  }
  code = morse_codes[idx];
  blink_code(led_pin, code);
}

void blink_morse_char_rus(int led_pin, char first, char second) {
  // Случай с большими символами Кириллицы.
  // Первый байт не трогаем ( он всегда 208 )
  // Второй байт от 144 до 175
  // В таблице ASCII же А-Я = 192-223
  // Значит нужно брать второй бит и вычитать 144, чтобы получить мап 0-31
  int idx = second - 144;
  const char* code = NULL;
  code = morse_codes_rus[idx];
  blink_code(led_pin, code);
}

void blink_code(int led_pin, const char* code) {
  // Now loop through the components of the code and display them.
  // Be careful to not delay after the last character.
  for (int i = 0; i < strlen(code)-1; ++i) {
     // Turn the LED on, delay for the appropriate dot / dash time,
     // then turn the LED off again.
     digitalWrite(led_pin, led_on);
     delay((code[i] == '.') ? morse_dot_ms : morse_dash_ms);
     digitalWrite(led_pin, led_off);
     // Delay between characters.
     delay(morse_char_delay_ms);
  }
  // Handle the last character without any delay after it.
  digitalWrite(led_pin, led_on);
  delay((code[strlen(code)-1] == '.') ? morse_dot_ms : morse_dash_ms);
  digitalWrite(led_pin, led_off);
}

void blink_morse(int led_pin, const char* message) {
  // Blink out the morse code version of the message on the LED.
  
  // Process each character in the message and send them out 
  // as morse code. Keep track of the previously seen character
  // to find word boundaries.
  int start_i = 1;
  char old = toupper(message[0]);
  if (is_russian(old)) {
    blink_morse_char_rus(led_pin, message[0], message[1]);
    start_i = 2;
  }
  else {
    blink_morse_char(led_pin, old);
  }
  for (int i = start_i; i < strlen(message); ++i) {
    char c = toupper(message[i]);
    // Delay for word boundary if last char is whitespace and 
    // new char is alphanumeric.
    if (isspace(old) && isalnum(c)) {
      delay(morse_word_delay_ms); 
    }
    // Delay for letter boundary if both last and new char 
    // are alphanumeric.
    else if (isalnum(old) && isalnum(c)) {
      delay(morse_letter_delay_ms);
    }
    
    // Если символ кириллицы - забираем 2 элемента, инкрементируем счетчик
    if (is_russian(c)) {
      blink_morse_char_rus(led_pin, c, message[i + 1]);
      old = message[i + 1];
      i++;
    }
    else {
      blink_morse_char(led_pin, c);
      old = c;
    }
  }
}

void form_url_decode(const char* encoded, char* decoded) {
  // Decode a string in 'application/x-www-form-urlencoded' format
  // to its normal ASCII representation.  
  // Note the output string MUST be large enough to hold the string!
  // As long as the output is at least as large as the input then
  // you will never have a problem (i.e. decoded strings are smaller).
  
  // Go through each character and trasform it into the decoded
  // output as appropriate.
  for (int i = 0, j = 0; i < strlen(encoded); ++i, ++j) {
    char c = encoded[i];
    // The + character turns into a space.
    if (c == '+') {
      decoded[j] = ' '; 
    }
    // The % character indicates a special character in the
    // form of '%HH' where HH are the hex digits that represent
    // the character to use.
    else if (c == '%') {
      // If there isn't enough input data to read the next two
      // characters then just stop because this string is malformed.
      if (i >= strlen(encoded)-2) {
        return; 
      }
      // Grab the next two encoded characters.
      char high = toupper(encoded[++i]);
      char low  = toupper(encoded[++i]);
      // Skip this character if the encoded characters aren't hex.
      if (!isxdigit(high) || !isxdigit(low)) {
        continue;
      }
      // Convert high and low hex digits to the character
      // represented by their value.
      decoded[j] = (low > '9') ? (low-'A')+10 : low-'0';
      decoded[j] += 16*((high > '9') ? (high-'A')+10 : high-'0');
    }
    // All other encoded characters are just copied over.
    else {
      decoded[j] = c;
    }
  } 
}

bool is_russian(char c) {
  //Проверка на русские символы
  if (int(c) == 208) {
    return true;
  }
  return false;
}
