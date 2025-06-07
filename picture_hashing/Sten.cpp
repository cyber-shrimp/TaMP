#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

using namespace std;

// ��������, ��� ������ �������� A-Z ��� a-z ��� ������
bool isLatinChar(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == ' ');
}

// �������� ��������� � BMP-����, �������� ����� BMP � ��������� ����������
bool encodeMessage(const string& inputBmpPath, const string& outputBmpPath, const string& message) {
    // ��������� ��������� �� ��������
    for (char c : message) {
        if (!isLatinChar(c)) {
            cerr << "Wrong alphabet." << endl;
            return false;
        }
    }

    // ��������� �������� BMP-����
    ifstream inputFile(inputBmpPath, ios::binary);
    if (!inputFile) {
        cerr << "I can't open your file." << endl;
        return false;
    }

    // ������ ���� ���� � ������
    vector<uint8_t> fileData((istreambuf_iterator<char>(inputFile)), {});
    inputFile.close();

    // ��������� ������ ����� (������� ��������� BMP)
    if (fileData.size() < 54) {
        cerr << "I don't think it is the right format." << endl;
        return false;
    }

    // �������� �������� ������ ������ ����������� �� ��������� (����� 10-13)
    uint32_t dataOffset = *reinterpret_cast<uint32_t*>(&fileData[10]);
    if (dataOffset >= fileData.size()) {
        cerr << "Error." << endl;
        return false;
    }

    // ���������� ��������� � ����������� ����
    string msgWithTerminator = message + '\0';

    // ������������ ������� ����� ����� ��������
    size_t bitsToWrite = msgWithTerminator.size() * 8;

    // ���������, ��� � ������ ����������� ���������� ������ ��� ������
    if (dataOffset + bitsToWrite > fileData.size()) {
        cerr << "Too long." << endl;
        return false;
    }

    // ��������� ��������� � ������� ���� ��������
    size_t bitIndex = 0;
    for (char c : msgWithTerminator) {
        for (int bit = 0; bit < 8; bit++) {
            uint8_t bitVal = (c >> bit) & 1;
            size_t dataPos = dataOffset + bitIndex;
            fileData[dataPos] = (fileData[dataPos] & 0xFE) | bitVal;
            bitIndex++;
        }
    }

    // ���������� ��������� � ����� ����
    ofstream outputFile(outputBmpPath, ios::binary);
    if (!outputFile) {
        cerr << "I can't make this file." << endl;
        return false;
    }

    outputFile.write(reinterpret_cast<char*>(fileData.data()), fileData.size());
    outputFile.close();

    cout << "Congratulations, your secrets are a little bit safer now here: " << outputBmpPath << endl;
    return true;
}

// ������� ��������� �� BMP (������ �������� + ������)
string decodeMessage(const string& filePath) {
    ifstream inputFile(filePath, ios::binary);
    if (!inputFile) {
        cerr << "Error: I can't open your image." << endl;
        return "";
    }

    // ������ ��������� BMP (54 �����)
    vector<uint8_t> header(54);
    inputFile.read(reinterpret_cast<char*>(header.data()), header.size());
    if (inputFile.gcount() != 54) {
        cerr << "Nope, this is not BMP." << endl;
        return "";
    }

    // ���������� ������ ������ �����������
    uint32_t dataOffset = *reinterpret_cast<uint32_t*>(&header[10]);

    // ��������� � ������ �����������
    inputFile.seekg(dataOffset, ios::beg);
    if (!inputFile) {
        cerr << "Error: I can't." << endl;
        return "";
    }

    // ������ ��� ������ ��������
    vector<uint8_t> pixelData((istreambuf_iterator<char>(inputFile)), {});
    if (pixelData.empty()) {
        cerr << "Seems like this is just the picture." << endl;
        return "";
    }

    string decodedMessage;
    uint8_t currentByte = 0;
    int bitIndex = 0;

    for (uint8_t pixelByte : pixelData) {
        uint8_t lsb = pixelByte & 1;
        currentByte |= (lsb << bitIndex);
        bitIndex++;

        if (bitIndex == 8) {
            if (currentByte == '\0') {
                // ������� ���� � ����� ���������
                break;
            }

            // ��������� ������: �������� ��� ������
            if ((currentByte >= 'A' && currentByte <= 'Z') ||
                (currentByte >= 'a' && currentByte <= 'z') ||
                (currentByte == ' ')) {
                decodedMessage += static_cast<char>(currentByte);
            }
            else {
                cerr << "Hmm, I see something I don't know." << endl;
                return "";
            }

            currentByte = 0;
            bitIndex = 0;
        }
    }

    return decodedMessage;
}

int main() {
    string filePath, inputPath, outputPath, message;
    int i;
    cout << "What is my purpose" << endl << "1. Encode" << endl << "2. Decode" << endl << "3. exit" << endl;
    cin >> i;
    switch (i) {
    case 1:
        
        cout << "What do you need to change: ";
        cin >> inputPath;

        cout << "Where do you need your image: ";
        cin >> outputPath;

        cout << "FEED ME YOUR SECRETS (only latin alphabet): ";
        cin.ignore(); // ���������� ������ ����� ������ ����� cin
        getline(cin, message);

        if (!encodeMessage(inputPath, outputPath, message)) {
            cerr << "Something went terribly wrong." << endl;
            return 1;
        }
        break;
    case 2:
        
        cout << "FEED ME YOUR SECRETS (BMP): ";
        cin >> filePath;

        cout << " " << filePath << endl;

        string message = decodeMessage(filePath);

        if (!message.empty()) {
            cout << "Congratulations, here is your message: " << message << endl;
        }
        else {
            cerr << "Something is wrong." << endl;
        }

    }
        
    

    return 0;
}
