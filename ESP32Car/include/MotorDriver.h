
struct MotorPinInfo {
    unsigned char mA1, mA2, mB1, mB2, mPWM, mS1, mS2;
};

enum class MotorMove{
    STOP = 0, FWRD = 1, BCKWRD, LFT, RGHT
};


class MotorDriver {

public:
    MotorDriver();
    MotorDriver(MotorPinInfo pins, bool hasSensor1, bool hasSensor2);
    void begin(MotorPinInfo pins, bool hasSensor1, bool hasSensor2);
    void setSpeed(unsigned char speed);
    unsigned char getSpeed();
    unsigned int getSensorCount(int sensorNum);
    void resetSensorCount(int sensorNum);
    void move(MotorMove dir);

private:
    static void sensor1();
    static void sensor2();

private:
    unsigned char m_motorPinA1, m_motorPinA2, m_motorPinB1, m_motorPinB2;
    unsigned char m_motorPinPWM;
    unsigned char m_motorSpeed = 128;
    bool m_hasSensor1 = false, m_hasSensor2 = false;
    unsigned int m_s1Count = 0, m_s2Count = 0;
    static MotorDriver* m_ref;

};
