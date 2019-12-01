#include "Huffman.h"

#include <queue>
#include <stack>
#include <map>
#include <string>
#include <iostream>

using std::priority_queue;
using std::vector;
using std::stack;
using std::map;
using std::endl;
using std::cout;

struct TNode {
    uint8_t data;
    uint32_t freq;
    TNode* left = nullptr;
    TNode* right = nullptr;

    TNode(uint8_t data_, uint32_t freq_) : data(data_), freq(freq_) {}
};

struct compare {
    bool operator()(TNode* l, TNode* r) {
        return (l->freq > r->freq);
    }
};

TNode* Huffman(const vector<byte>& data, const vector<uint32_t>& freq) {
    TNode *left, *right, *top;

    priority_queue<TNode*, vector<TNode*>, compare> minQueue;
    for (size_t i = 0; i < data.size(); ++i) {
        minQueue.push(new TNode(data[i], freq[i]));
    }

    while (minQueue.size() != 1) {
        left = minQueue.top();
        minQueue.pop();
        right = minQueue.top();
        minQueue.pop();

        top = new TNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        minQueue.push(top);
    }

    return minQueue.top();
}

class BitsWriter {
public:
    void WriteBit(bool bit);
    void WriteByte(byte Byte);

    std::vector<byte> GetResult();

private:
    std::vector<byte> buffer_;
    byte accumulator_ = 0;
    int bits_count_ = 0;
};

void BitsWriter::WriteBit(bool bit) {
    // Ставим бит в аккумулятор на нужное место
    accumulator_ |= static_cast<byte>(bit) << bits_count_;
    ++bits_count_;
    if (bits_count_ == 8) {
        bits_count_ = 0;
        buffer_.push_back(accumulator_);
        accumulator_ = 0;
    }
}

void BitsWriter::WriteByte(byte Byte) {
    if (bits_count_ == 0) {
        buffer_.push_back(Byte);
    } else {
        accumulator_ |= Byte << bits_count_;
        buffer_.push_back(accumulator_);
        accumulator_ = Byte >> (8 - bits_count_);
    }
}

std::vector<byte> BitsWriter::GetResult() {
    if (bits_count_ != 0) {
        // Добавляем в буфер аккумулятор, если в нем что-то есть.
        buffer_.push_back(accumulator_);
    }
    buffer_.push_back(static_cast<byte>(bits_count_));
    return std::move(buffer_);
}

void writeTreeHeader(const vector<byte>& items, vector<uint32_t>& freqs, IOutputStream& output) {
    uint32_t size = items.size();
    for (size_t j = 0; j < 4; ++j) {
        output.Write(static_cast<byte*>(static_cast<void*>(&size))[j]);
    }
    for (size_t i = 0; i < size; ++i) {
        output.Write(items[i]);
        for (size_t j = 0; j < 4; ++j) {
            output.Write(static_cast<byte*>(static_cast<void*>(&(freqs[i])))[j]);
        }
    }
}

void readTreeHeader(vector<byte>& items, vector<uint32_t>& freqs, IInputStream& input) {
    uint32_t size, freq;
    byte tmp;
    for (size_t j = 0; j < 4; ++j) {
        if (input.Read(tmp)) {
            *(static_cast<byte*>(static_cast<void*>(&size)) + j) = tmp;
        }
    }
    for (size_t i = 0; i < size; ++i) {
        input.Read(tmp);
        items.push_back(tmp);
        for (size_t j = 0; j < 4; ++j) {
            if (input.Read(tmp)) {
                *(static_cast<byte*>(static_cast<void*>(&freq)) + j) = tmp;
            }
        }
        freqs.push_back(freq);
    }
}

void buildTreeMapping(map<byte, std::string>& mapper, TNode* node, std::string prefix) {
    if (node->left == nullptr && node->right == nullptr) {
        // leaf
        mapper[node->data] = prefix;
    } else {
        buildTreeMapping(mapper, node->left, prefix + 'l');
        buildTreeMapping(mapper, node->right, prefix + 'r');
    }
}

void buildTreeReverseMapping(map<std::string, byte>& mapper, TNode* node, std::string prefix) {
    if (node->left == nullptr && node->right == nullptr) {
        // leaf
        mapper[prefix] = node->data;
    } else {
        buildTreeReverseMapping(mapper, node->left, prefix + 'l');
        buildTreeReverseMapping(mapper, node->right, prefix + 'r');
    }
}

void printDataCompressed(const vector<byte>& input, TNode* tree, BitsWriter& writer) {
    map<byte, std::string> mapper;
    buildTreeMapping(mapper, tree, "");
    for (const auto& el : input) {
        for (const auto& c : mapper[el]) writer.WriteBit(c == 'r');
    }
}

void Encode(IInputStream& original, IOutputStream& compressed) {
    byte tmp;
    vector<byte> input;
    vector<uint32_t> alphabet(256);
    while (original.Read(tmp)) {
        alphabet[tmp]++;
        input.push_back(tmp);
    }
    // build tree
    vector<byte> items;
    vector<uint32_t> freqs;
    for (uint32_t i = 0; i <= 255; ++i) {
        if (alphabet[i] > 0) {
            items.push_back(static_cast<byte>(i));
            freqs.push_back(alphabet[i]);
        }
    }
    auto* tree = Huffman(items, freqs);
    // write tree
    writeTreeHeader(items, freqs, compressed);
    // write data
    auto size = static_cast<uint32_t>(input.size());
    for (size_t j = 0; j < 4; ++j) {
        compressed.Write(static_cast<byte*>(static_cast<void*>(&size))[j]);
    }
    BitsWriter writer;
    printDataCompressed(input, tree, writer);
    auto res = writer.GetResult();
    for (size_t i = 0; i < res.size(); ++i) {
        compressed.Write(res[i]);
    }
}

void Decode(IInputStream& compressed, IOutputStream& original) {
    vector<byte> items;
    vector<uint32_t> freqs;
    readTreeHeader(items, freqs, compressed);
    // build tree
    auto* huffTree = Huffman(items, freqs);
    // read header
    map<std::string, byte> mapper;
    buildTreeReverseMapping(mapper, huffTree, "");
    uint32_t size;
    byte tmp;
    for (size_t j = 0; j < 4; ++j) {
        if (compressed.Read(tmp)) {
            *(static_cast<byte*>(static_cast<void*>(&size)) + j) = tmp;
        }
    }
    // read data
    std::string buff;
    uint32_t written = 0;
    while (compressed.Read(tmp) && written != size) {
        for (int16_t i = 0; i < 8; ++i) {
            buff += (tmp & (0x1 << i)) ? 'r' : 'l';
            if (mapper.find(buff) != mapper.end()) {
                original.Write(mapper[buff]);
                written++;
                buff = "";
            }
            if (written == size) {
                break;
            }
        }
    }
}

class TReader : public IInputStream {
private:
    size_t Position = 0;
    const std::vector<byte> InputLine;
public:
    TReader(const std::string& input) : InputLine(input.begin(), input.end()) {}
    TReader(const vector<byte>& input) : InputLine(input) {}

    bool Read(byte& value) override {
        if (Position < InputLine.size()) {
            value = InputLine[Position++];
            return true;
        }
        return false;
    }
};

class TWriter : public IOutputStream {
private:
    std::vector<byte> Line;
public:
    TWriter() {}

    void Write(byte value) override {
        Line.push_back(value);
    }

    const std::vector<byte>& GetResult() {
        return Line;
    }
};

int main() {
    TReader reader("abbbbbccdeeeeeeeeeeefqq");
    TWriter writer1, writer2;
    Encode(reader, writer1);
    TReader reader2(writer1.GetResult());
    std::cout << "WTF " << std::string(writer1.GetResult().begin(), writer1.GetResult().end()) << std::endl;
    Decode(reader2, writer2);
    std::cout << std::string(writer2.GetResult().begin(), writer2.GetResult().end()) << std::endl;
}
