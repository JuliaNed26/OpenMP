#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>
#include <vector>

#define NUM_THREADS 50
#define SHIFT 1

std::string caesarEncrypt(std::string text, int s) {
    std::string result = "";

    for (char c : text) {
        if (isupper(c))
            result += char(int(c + s - 'A') % 26 + 'A');
        else if (islower(c))
            result += char(int(c + s - 'a') % 26 + 'a');
        else
            result += c;
    }

    return result;
}

void readAndEncryptFile() {
    const char* file_name = "wordsToEncrypt.txt";
    std::string text;
    std::ifstream in_file(file_name);

    if (!in_file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    std::vector<std::string> encryptedChunks(NUM_THREADS);

    // Read entire file into a string
    std::string line;
    while (std::getline(in_file, line)) {
        text += line + "\n";
    }
    in_file.close();

    double start = omp_get_wtime();

    int length = text.length();
    int chunk_size = length / NUM_THREADS;
    omp_set_num_threads(NUM_THREADS);

#pragma omp parallel
    {
        int ID = omp_get_thread_num();
        int start = ID * chunk_size;
        int end = (ID == NUM_THREADS - 1) ? length : start + chunk_size;
        std::string substring = text.substr(start, end - start);
        std::string encrypted = caesarEncrypt(substring, SHIFT);
        encryptedChunks[ID] = encrypted;
    }

    double end = omp_get_wtime();
    std::cout << "Time elapsed " << end - start << std::endl;

    // Write encrypted chunks to file
    std::ofstream out_file("encryptedText.txt", std::ios::out);
    if (out_file.is_open()) {
        for (const auto& chunk : encryptedChunks) {
            out_file << chunk;
        }
        out_file.close();
    }
    else {
        std::cerr << "Error writing to file." << std::endl;
    }
}

int main() {
    readAndEncryptFile();
    return 0;
}
