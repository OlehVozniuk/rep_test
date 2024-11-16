void setup() {
  Serial.begin(9600);  // Налаштування серіальної передачі на швидкості 9600 бод
  while (!Serial) {
    ; // Чекаємо на підключення серіального порту
  }
  Serial.println("Arduino ready to receive data");  // Стартове повідомлення
}

void loop() {
  // Перевіряємо, чи є доступні дані для читання
  if (Serial.available() > 0) {
    // Читаємо повідомлення від клієнта (до символу '\n')
    String receivedMessage = Serial.readStringUntil('\n');

    // Створюємо змінну для збереження перевернутого рядка
    String reversedMessage = "";

    // Зворотний порядок символів
    for (int i = receivedMessage.length() - 1; i >= 0; i--) {
      reversedMessage += receivedMessage[i];
    }

    // Відправляємо перевернуте повідомлення клієнту
    Serial.println(reversedMessage);
  }
}
