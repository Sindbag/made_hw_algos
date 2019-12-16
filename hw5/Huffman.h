typedef unsigned char byte;

struct IInputStream {
    // Возвращает false, если поток закончился
    virtual bool Read(byte& value) { return false; };
};

struct IOutputStream {
    virtual void Write(byte value) {};
};
