
#include "displayLED.h"

//Store digit displays
static const unsigned char digitDisplay[10][3] = {
    {0x1F, 0xA0, 0x1F}, // 0
    {0x20, 0xBF, 0x21}, // 1
    {0X23, 0xA4, 0x39}, // 2
    {0x1B, 0xA4, 0xA4}, // 3
    {0xBF, 0x04, 0x87}, // 4
    {0x98, 0xA4, 0x97}, // 5
    {0x98, 0xA4, 0x1F}, // 6
    {0x83, 0x84, 0xB8}, // 7
    {0x1B, 0xA4, 0x1B}, // 8
    {0x3F, 0x84, 0x03}  // 9
};

//Initialize i2c bus
int initI2cBus(char* bus, int address){
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

//Write to slave register
void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value){
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2){
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

//Initialize 8x8 LED Matrix registers
void initDisplay(){
    //init LED Display
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS2, LED_DISPLAY_ADDRESS);
    //turn on LED matrix through System Setup register
    writeI2cReg(i2cFileDesc, 0x21, 0x00);
    //Set display to ON
    writeI2cReg(i2cFileDesc, 0x81, 0x00);
}

//Display inputted integer on 8x8 LED Matrix
void displayIntVal(int numToDisplay){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS2, LED_DISPLAY_ADDRESS);

    //Check for valid values between 0 & 99
    if(numToDisplay < 0){
        numToDisplay = 0;
    }
    else if(numToDisplay > 99){
        numToDisplay = 99;
    }

    //Seperate tens digit from ones digit
    int tensDigit = numToDisplay / 10;
    int onesDigit = numToDisplay % 10;

    //Displays ten digit if integer is greater than 9
    if(tensDigit != 0){
        for(int i = 0; i < 3; ++i){
            writeI2cReg(i2cFileDesc, (i+4)*2, digitDisplay[tensDigit][i]);
        }
    }
    //Checks for values not between 0 & 99 
    else if(tensDigit > 9){
        for(int i = 0; i < 3; ++i){
            writeI2cReg(i2cFileDesc, (i+4)*2, digitDisplay[9][i]);
        }
    }
    else if(tensDigit <= 0){
        for(int i = 0; i < 3; ++i){
            writeI2cReg(i2cFileDesc, (i+4)*2, digitDisplay[0][i]);
        }
    }

    //Display ones digit
    for(int i = 0; i < 4; ++i){
        writeI2cReg(i2cFileDesc, i*2, digitDisplay[onesDigit][i]);
    }
}

//Display inputted Double on 8x8 LED Matrix
void displayDoubleVal(double numToDisplay){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS2, LED_DISPLAY_ADDRESS);

    //Check for valid values between 0.0 & 9.9
    if(numToDisplay < 0.0){
        numToDisplay = 0.0;
    }
    else if(numToDisplay > 9.9){
        numToDisplay = 9.9;
    }

    //Seperate whole digit from decimal digit
    int wholeDigit = (int)numToDisplay;
    int decimalDigit = (int)((numToDisplay - wholeDigit) * 10);

    //Displays whole digit if integer is greater than 0.9
    if(wholeDigit != 0){
        for(int i = 0; i < 3; ++i){
            writeI2cReg(i2cFileDesc, (i+4)*2, digitDisplay[wholeDigit][i]);
        }
    }
    //Checks for values not between 0.0 & 9.9 
    else if(wholeDigit > 9){
        for(int i = 0; i < 3; ++i){
            writeI2cReg(i2cFileDesc, (i+4)*2, digitDisplay[9][i]);
        }
    }
    else if(wholeDigit <= 0){
        for(int i = 0; i < 3; ++i){
            writeI2cReg(i2cFileDesc, (i+4)*2, digitDisplay[0][i]);
        }
    }

    //Display ones digit
    for(int i = 0; i < 3; ++i){
        writeI2cReg(i2cFileDesc, i*2, digitDisplay[decimalDigit][i]);
    }

    //Display Decimal Point
    writeI2cReg(i2cFileDesc, 6, 0x40);
}
