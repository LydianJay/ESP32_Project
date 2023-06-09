#include "MotorDriver.h"
#include <Arduino.h>

MotorDriver* MotorDriver::m_ref = nullptr;

MotorDriver::MotorDriver(){
    m_ref = this;
}

MotorDriver::MotorDriver(MotorPinInfo pins, bool hasSensor1, bool hasSensor2) {
    constexpr unsigned int _PWM_CHNL = 0;
    constexpr unsigned int _PWM_FREQ = 1000;
    constexpr unsigned int _PWM_CH = 0;
    constexpr unsigned int _PWN_RES = 8;

    pinMode(pins.mA1, OUTPUT);
    pinMode(pins.mA2, OUTPUT);
    pinMode(pins.mB1, OUTPUT);
    pinMode(pins.mB2, OUTPUT);
    pinMode(pins.mA1, OUTPUT);
    pinMode(pins.mA2, OUTPUT);
    pinMode(pins.mPWM, OUTPUT);

    digitalWrite(pins.mA1, LOW);
    digitalWrite(pins.mA2, LOW);
    digitalWrite(pins.mB1, LOW);
    digitalWrite(pins.mB2, LOW);

    ledcSetup(_PWM_CH, _PWM_FREQ, _PWN_RES);
    ledcAttachPin(pins.mPWM, _PWM_CH);
    ledcWrite(_PWM_CH, m_motorSpeed);

    if(hasSensor1) {
        m_hasSensor1 = true;
        pinMode(pins.mS1, INPUT_PULLUP);
        attachInterrupt(pins.mS1, sensor1, RISING);
    }

    if(hasSensor2) {
        m_hasSensor2 = true;
        pinMode(pins.mS2, INPUT_PULLUP);
        attachInterrupt(pins.mS2, sensor2, RISING);
    }

    m_ref = this;
}

void MotorDriver::begin(MotorPinInfo pins, bool hasSensor1, bool hasSensor2) {
    m_ref = this;
    m_motorPinA1 = pins.mA1; m_motorPinA2 = pins.mA2;
    m_motorPinB1 = pins.mB1; m_motorPinB2 = pins.mB2;
    m_motorPinPWM = pins.mPWM;

    constexpr unsigned int _PWM_CHNL = 0;
    constexpr unsigned int _PWM_FREQ = 1000;
    constexpr unsigned int _PWM_CH = 0;
    constexpr unsigned int _PWN_RES = 8;

    pinMode(pins.mA1, OUTPUT);
    pinMode(pins.mA2, OUTPUT);
    pinMode(pins.mB1, OUTPUT);
    pinMode(pins.mB2, OUTPUT);
    pinMode(pins.mA1, OUTPUT);
    pinMode(pins.mA2, OUTPUT);
    pinMode(pins.mPWM, OUTPUT);

    digitalWrite(pins.mA1, LOW);
    digitalWrite(pins.mA2, LOW);
    digitalWrite(pins.mB1, LOW);
    digitalWrite(pins.mB2, LOW);

    ledcSetup(_PWM_CH, _PWM_FREQ, _PWN_RES);
    ledcAttachPin(pins.mPWM, _PWM_CH);
    ledcWrite(_PWM_CH, m_motorSpeed);

    if(hasSensor1) {
        m_hasSensor1 = true;
        pinMode(pins.mS1, INPUT_PULLUP);
        attachInterrupt(pins.mS1, sensor1, RISING);
    }

    if(hasSensor2) {
        m_hasSensor2 = true;
        pinMode(pins.mS2, INPUT_PULLUP);
        attachInterrupt(pins.mS2, sensor2, RISING);
    }
}

void MotorDriver::setSpeed(unsigned char speed) {
    m_motorSpeed = speed;
}

unsigned char MotorDriver::getSpeed() {
    return m_motorSpeed;
}

unsigned int MotorDriver::getSensorCount(int sensorNum) {
    switch (sensorNum){
        case 0:
            return m_s1Count;
        break;
        case 1:
            return m_s2Count;
        break;
        
    }
    return 0;
}

void MotorDriver::resetSensorCount(int sensorNum) {
    switch (sensorNum){
        case 0:
            m_s1Count = 0;
        break;
        case 1:
            m_s2Count = 0;
        break;
    }
}

void MotorDriver::move(MotorMove dir) {


    switch(dir){
        case MotorMove::STOP:
            digitalWrite(m_motorPinA1, LOW);
            digitalWrite(m_motorPinA2, LOW);
            digitalWrite(m_motorPinB1, LOW);
            digitalWrite(m_motorPinB2, LOW);
                
        break;

        case MotorMove::FWRD:
            digitalWrite(m_motorPinA1, HIGH);
            digitalWrite(m_motorPinA2, LOW);
            digitalWrite(m_motorPinB1, HIGH);
            digitalWrite(m_motorPinB2, LOW);
        break;


        case MotorMove::BCKWRD:
            digitalWrite(m_motorPinA1, LOW);
            digitalWrite(m_motorPinA2, HIGH);
            digitalWrite(m_motorPinB1, LOW);
            digitalWrite(m_motorPinB2, HIGH);  
        break;


        case MotorMove::LFT:
            digitalWrite(m_motorPinA1, HIGH);
            digitalWrite(m_motorPinA2, LOW);
            digitalWrite(m_motorPinB1, LOW);
            digitalWrite(m_motorPinB2, HIGH);
        break;


        case MotorMove::RGHT:
            digitalWrite(m_motorPinA1, LOW);
            digitalWrite(m_motorPinA2, HIGH);
            digitalWrite(m_motorPinB1, HIGH);
            digitalWrite(m_motorPinB2, LOW);
        break;
    }




}

void MotorDriver::sensor1() {
    if(m_ref!=nullptr)
        m_ref->m_s1Count++;
}

void MotorDriver::sensor2() {
    if(m_ref!=nullptr)
        m_ref->m_s2Count++;
}
