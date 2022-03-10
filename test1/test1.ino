/*
三个pin同时输出不同的高低电平


*/


#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define PIN_A 15
#define PIN_B 0
#define PIN_C 25

// define two tasks
void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);

// the setup function runs once when you press reset or power the board
void setup()
{
    pinMode(PIN_A, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    pinMode(PIN_C, OUTPUT);

    // initialize serial communication at 115200 bits per second:
    Serial.begin(115200);

    // Now set up two tasks to run independently.
    xTaskCreatePinnedToCore(
        Task1, "Task1" // A name just for humans
        ,
        1024 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(Task2, "Task2", 1024, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task3, "Task3", 1024, NULL, 2, NULL, ARDUINO_RUNNING_CORE);

    // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
    // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void Task1(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    pinMode(PIN_A, OUTPUT);

    while (1) // A Task shall never return or exit.
    {
        digitalWrite(PIN_A, HIGH);
        vTaskDelay(10);
        digitalWrite(PIN_A, LOW);
        vTaskDelay(20);
        digitalWrite(PIN_A, HIGH);
        vTaskDelay(30);
        digitalWrite(PIN_A, LOW);
        vTaskDelay(40);
    }
}

void Task2(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    pinMode(PIN_B, OUTPUT);

    while (1) // A Task shall never return or exit.
    {
        digitalWrite(PIN_B, HIGH);
        vTaskDelay(50);
        digitalWrite(PIN_B, LOW);
        vTaskDelay(50);
    }
}


void Task3(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    pinMode(PIN_C, OUTPUT);

    while (1) // A Task shall never return or exit.
    {
        digitalWrite(PIN_C, HIGH);
        vTaskDelay(30);
        digitalWrite(PIN_C, LOW);
        vTaskDelay(20);
    }
}
