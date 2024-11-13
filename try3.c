#include <LPC213x.h>

#define LCD_RS (1 << 8)       // RS connected to P0.8
#define LCD_EN (1 << 9)       // EN connected to P0.9
#define LCD_DATA (0xFF << 16) // Data lines connected to P0.16 - P0.23

// Function Prototypes
void UART0_Init(void);
void UART0_SendChar(char ch);
char UART0_ReceiveChar(void);
void UART0_SendString(char *str);
void LCD_Init(void);
void LCD_Command(unsigned char cmd);
void LCD_Data(unsigned char data);
void LCD_DisplayChar(char ch);
void Delay(unsigned int ms);

// Function to initialize UART0
void UART0_Init(void)
{
    PINSEL0 |= (1 << 0) | (1 << 2); // Enable TXD0 and RXD0 pins (P0.0 and P0.1)
    U0LCR = 0x83;                   // Enable DLAB, 8-bit data length
    U0DLM = 0x00;                   // Set high byte of baud rate divisor
    U0DLL = 0x61;                   // Set baud rate to 9600 for 15MHz clock
    U0LCR = 0x03;                   // Disable DLAB, set 8-bit data length
    U0FCR = 0x07;                   // Enable and reset RX and TX FIFO
}

// Function to send a command to the LCD
void LCD_Command(unsigned char cmd)
{
    IOCLR0 = LCD_RS;      // RS = 0 for command mode
    IOSET0 = LCD_EN;      // EN = 1
    IOCLR0 = LCD_DATA;    // Clear data lines
    IOSET0 = (cmd << 16); // Send command (8-bit mode)
    Delay(2);
    IOCLR0 = LCD_EN; // EN = 0
    Delay(2);
}

// Function to send data to the LCD
void LCD_Data(unsigned char data)
{
    IOSET0 = LCD_RS;       // Set RS for data mode
    IOSET0 = LCD_EN;       // Enable LCD
    IOCLR0 = LCD_DATA;     // Clear data lines
    IOSET0 = (data << 16); // Send data (8-bit mode)
    Delay(2);              // Delay for processing
    IOCLR0 = LCD_EN;       // Disable LCD
    Delay(2);              // Delay after disabling
}

// Function to initialize the LCD in 8-bit mode
void LCD_Init(void)
{
    IO0DIR |= LCD_RS | LCD_EN | LCD_DATA; // Set P0.8-P0.9 and P0.16-P0.23 as output
    Delay(50);                            // Wait for LCD to power up

    // Initialize LCD in 8-bit mode
    LCD_Command(0x38); // 8-bit mode, 2-line, 5x7 font
    Delay(5);
    LCD_Command(0x0C); // Display ON, cursor OFF
    Delay(5);
    LCD_Command(0x80); // Entry mode set, increment cursor
    Delay(5);
    LCD_Command(0x01); // Clear display
    Delay(5);
}

// Function to display a character on the LCD
void LCD_DisplayChar(char ch)
{
    LCD_Data(ch); // Send the character as data
}

// Simple delay function
void Delay(unsigned int ms)
{
    unsigned int i;
    for (i = 0; i < ms * 1000; i++)
        ; // Simple busy-wait delay
}

// Main function
int main(void)
{
    char receivedChar;
		IODIR1 |= (1<<16);
    VPBDIV = 0x01; //CCLK = PCLK
    UART0_Init(); // Initialize UART0
    LCD_Init();   // Initialize LCD
    while (1)
    {
        receivedChar = UART0_ReceiveChar(); // Receive a character from UART (Bluetooth)
        if (receivedChar == '#')
        {
            LCD_Command(0x01); //Clear LCD
            Delay(5);
            continue;
        }
        else if (receivedChar == '%')
        {
					// Buzzer 
						IOSET1 |= (1<<16);
						Delay(150);
						IOCLR1 |= (1<<16);
						Delay(150);
						IOSET1 |= (1<<16);
						Delay(150);
						IOCLR1 |= (1<<16);
						Delay(150);
						IOSET1 |= (1<<16);
						Delay(150);
						IOCLR1 |= (1<<16);
						Delay(150);
            continue;
        }
        else if (receivedChar == '@')
        {
            LCD_Command(0xC0);
            continue;
        }
        else if (receivedChar == '&')
        {
            LCD_Command(0x90);
            continue;
        }
        else if (receivedChar == '*')
        {
            LCD_Command(0xD0);
            continue;
        }
        else
        {
            LCD_DisplayChar(receivedChar); // Display the received character on the LCD
        }
    }
}

// Function to receive a character from UART0
char UART0_ReceiveChar(void)
{
    while (!(U0LSR & 0x01));         // Wait until data is available in the RX FIFO
    return U0RBR; // Return received character
}
