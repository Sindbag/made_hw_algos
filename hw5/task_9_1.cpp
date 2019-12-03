#include "Huffman.h"

#include <queue>
#include <stack>
#include <map>
#include <string>
#include <iostream>
#include <utility>
#include <cmath>
#include <cassert>

namespace {
    using std::priority_queue;
    using std::queue;
    using std::vector;
    using std::stack;
    using std::map;
    using std::endl;
    using std::cout;

    class BitsWriter {
    public:
        void WriteBit(bool bit);

        std::vector<byte> GetResult();

    private:
        std::vector<byte> buffer_;
        byte accumulator_ = 0;
        int bits_count_ = 0;
    };

    void BitsWriter::WriteBit(bool bit) {
        // Ставим бит в аккумулятор на нужное место
        accumulator_ |= static_cast<unsigned char>(bit) << bits_count_++;
        if (bits_count_ == 8) {
            bits_count_ = 0;
            buffer_.push_back(accumulator_);
            accumulator_ = 0;
        }
    }

    std::vector<byte> BitsWriter::GetResult() {
        if (bits_count_ != 0) {
            // Добавляем в буфер аккумулятор, если в нем что-то есть.
            buffer_.push_back(accumulator_);
        }
        return std::move(buffer_);
    }

    void readBytes(IInputStream& input, size_t size, void* dest) {
        byte tmp;
        for (size_t j = 0; j < size; ++j) {
            if (input.Read(tmp)) {
                *(static_cast<byte*>(dest) + j) = tmp;
            }
        }
    }

    void writeBytes(IOutputStream& output, size_t size, void* from) {
        for (size_t j = 0; j < size; ++j) {
            output.Write(static_cast<byte*>(from)[j]);
        }
    }

    class THuffmanTree {
    private:
        struct TNode {
            uint8_t data;
            uint32_t freq;
            TNode* left = nullptr;
            TNode* right = nullptr;

            TNode(uint8_t data_, uint32_t freq_) : data(data_), freq(freq_) {}

            void buildMapping(map<byte, std::string>& mapper, const std::string& prefix) const {
                if (left == nullptr && right == nullptr) {
                    mapper[data] = prefix;
                } else {
                    if (left != nullptr) left->buildMapping(mapper, prefix + 'l');
                    if (right != nullptr) right->buildMapping(mapper, prefix + 'r');
                }
            }

            void buildReverseMapping(map<std::string, byte>& mapper, const std::string& prefix) const {
                if (left == nullptr && right == nullptr) {
                    mapper[prefix] = data;
                } else {
                    if (left != nullptr) left->buildReverseMapping(mapper, prefix + 'l');
                    if (right != nullptr) right->buildReverseMapping(mapper, prefix + 'r');
                }
            }
        };

        struct compare {
            bool operator()(TNode* l, TNode* r) {
                return (l->freq > r->freq);
            }
        };

        TNode* root = nullptr;
        vector<byte> items;
        vector<uint32_t> freqs;

    public:
        THuffmanTree() = default;

        THuffmanTree(vector<byte> items_, vector<uint32_t> freqs_)
                : items(std::move(items_)), freqs(std::move(freqs_)) {
            constructTree();
        }

        THuffmanTree(IInputStream& input) {
            deserialize(input);
        }

        THuffmanTree(const vector<uint32_t>& alphabet) {
            for (uint32_t i = 0; i <= 255; ++i) {
                if (alphabet[i] > 0) {
                    items.push_back(static_cast<byte>(i));
                    freqs.push_back(alphabet[i]);
                }
            }
            constructTree();
        }

        THuffmanTree(const vector<byte>& data) : THuffmanTree(getAlphabet(data)) {}

        ~THuffmanTree() {
            queue<TNode*> nodes;
            nodes.push(root);
            while (!nodes.empty()) {
                TNode* top = nodes.front();
                nodes.pop();
                if (top == nullptr) continue;
                nodes.push(top->left);
                nodes.push(top->right);
                delete top;
            }
        }

        size_t getSize() const {
            size_t sum = 0;
            for (const auto& el: freqs) {
                sum += el;
            }
            return sum;
        }

        std::string getSerialString(vector<byte>& its) const {
            assert(root != nullptr);

            std::string path;
            map<TNode*, bool> visits;
            std::vector<TNode*> currStack;
            currStack.push_back(root);
            while (!currStack.empty()) {
                if (!currStack.back()->left && !currStack.back()->right) {
                    its.push_back(currStack.back()->data);
                }
                if (currStack.back()->left && !visits[currStack.back()->left]) {
                    path += 'd';
                    currStack.push_back(currStack.back()->left);
                    continue;
                }
                if (currStack.back()->right && !visits[currStack.back()->right]) {
                    path += 'd';
                    currStack.push_back(currStack.back()->right);
                    continue;
                }
                visits[currStack.back()] = true;
                currStack.pop_back();
                path += 'u';
            }
            std::string tmpPath;
            for (size_t i = 1; i < path.size(); ++i) {
                if (path[i] == 'd') {
                    tmpPath += path[i - 1] == 'u' ? 'u' : 'd';
                }
            }
            return tmpPath;
        }

        void serialize(IOutputStream& output) {
            vector<byte> its;
            std::string path = getSerialString(its);
            assert(its.size() == items.size());

            uint8_t size = its.size();
            writeBytes(output, sizeof(size), static_cast<void*>(&size));
            for (size_t i = 0; i < size; ++i) {
                output.Write(its[i]);
            }

            uint16_t psize = path.size();
            writeBytes(output, sizeof(psize), static_cast<void*>(&psize));
            BitsWriter writer;
            for (const char& p : path) {
                writer.WriteBit(p == 'd');
            }

            auto res = writer.GetResult();
            assert(res.size() == uint16_t(ceil(psize * 1. / 8)));
            for (unsigned char re : res) {
                output.Write(re);
            }
        }

        [[nodiscard]]
        map<byte, std::string> getMappings() const {
            map<byte, std::string> mapper;
            if (root != nullptr) root->buildMapping(mapper, "");
            return mapper;
        }

        [[nodiscard]]
        map<std::string, byte> getReverseMappings() const {
            map<std::string, byte> mapper;
            if (root != nullptr) root->buildReverseMapping(mapper, "");
            return mapper;
        }

    private:
        static vector<uint32_t> getAlphabet(const vector<byte>& data) {
            vector<uint32_t> alphabet(256);
            for (auto el: data) {
                alphabet[el]++;
            }
            return alphabet;
        }

        void constructTree() {
            TNode* left, * right, * top;

            priority_queue<TNode*, vector<TNode*>, compare> minQueue;
            for (size_t i = 0; i < items.size(); ++i) {
                minQueue.push(new TNode(items[i], freqs[i]));
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

            root = minQueue.top();
        }

        void deserialize(IInputStream& input) {
            uint8_t size;

            byte tmp;
            readBytes(input, sizeof(size), static_cast<void*>(&size));
            for (size_t i = 0; i < size; ++i) {
                input.Read(tmp);
                items.push_back(tmp);
            }

            std::string path = "d";
            uint16_t psize;
            readBytes(input, sizeof(psize), static_cast<void*>(&psize));
            const auto steps = static_cast<uint16_t>(ceil(psize * 1. / 8));
            for (uint16_t i = 0; i < steps; i++) {
                input.Read(tmp);
                for (uint8_t j = 0; (j < 8) && psize--; j++) {
                    path += (tmp & 0x1 << j) ? 'd' : 'u';
                }
            }
            path += "u";

            root = new TNode('$', 0);
            constructFromString(path);
        }

        void constructFromString(const std::string& path) {
            vector<TNode*> currStack = { root };
            TNode* curr = currStack.back();
            size_t q = 0;
            for (const char& pos : path) {
                if (pos == 'u') {
                    curr->data = items[q++];

                    while (curr != root) {
                        currStack.pop_back();
                        curr = currStack.back();
                        if (curr && !curr->right) {
                            break;
                        }
                    }

                    if (curr && !curr->right) {
                        curr->right = new TNode('$', 0);
                        curr = curr->right;
                        currStack.push_back(curr);
                    }
                }
                if (pos == 'd') {
                    curr->left = new TNode('$', 0);
                    curr = curr->left;
                    currStack.push_back(curr);
                }
            }
        }
    };

    class TEncoder {
    private:
        THuffmanTree* tree;
    public:
        TEncoder() = default;

        void encode(IInputStream& input, IOutputStream& output) {
            byte tmp;
            vector<byte> data;

            while (input.Read(tmp)) data.push_back(tmp);
            tree = new THuffmanTree(data);
            tree->serialize(output);

            uint32_t size = data.size();
            writeBytes(output, sizeof(size), static_cast<void*>(&size));

            auto compressedData = compress(data);
            for (const auto& chunk : compressedData) {
                output.Write(chunk);
            }
        }

    private:
        vector<byte> compress(const vector<byte>& data) {
            map<byte, std::string> mapper = tree->getMappings();

            BitsWriter writer;
            for (const auto& el : data) {
                for (const auto& c : mapper[el]) {
                    writer.WriteBit(c == 'r');
                }
            }

            return writer.GetResult();
        }
    };

    class TDecoder {
    private:
        THuffmanTree* tree;
    public:
        TDecoder() = default;

        void decode(IInputStream& input, IOutputStream& output) {
            tree = new THuffmanTree(input);
            uint32_t size;
            readBytes(input, sizeof(size), static_cast<void*>(&size));
            decompress(input, output, size);
        }

    private:
        void decompress(IInputStream& input, IOutputStream& output, uint32_t size) {
            map<std::string, byte> mapper = tree->getReverseMappings();
            std::string buff;
            byte tmp, step = 8;
            while (size) {
                if (step == 8) {
                    if (!input.Read(tmp)) break;
                    step = 0;
                }

                buff += (tmp & (0x1 << step++)) ? 'r' : 'l';
                if (mapper.find(buff) == mapper.end()) {
                    continue;
                }

                output.Write(mapper[buff]);
                buff = "";
                size--;
            }
        }
    };

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
}

void Encode(IInputStream& original, IOutputStream& compressed) {
    TEncoder encoder{};
    encoder.encode(original, compressed);
}

void Decode(IInputStream& compressed, IOutputStream& original) {
    TDecoder decoder{};
    decoder.decode(compressed, original);
}

int main() {
    std::string inp("missisipimissisipiasadpajsdpjadjpas - SOMETHING WRONG - SOMETHING WRONG");

    TReader reader(inp);
    TWriter writer1;
    Encode(reader, writer1);
    std::string middle(writer1.GetResult().begin(), writer1.GetResult().end());
    std::cout << middle << std::endl;

    TReader reader2(writer1.GetResult());
    TWriter writer2;
    Decode(reader2, writer2);
    std::string out(writer2.GetResult().begin(), writer2.GetResult().end());
    std::cout << out << endl;

    std::cout << (out == inp ? "- SAME" : "- SOMETHING WRONG") << std::endl;
}
