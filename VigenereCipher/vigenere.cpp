#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

using namespace std;

// Friedman test
float friedmanTest(const string& cipher)
{
    const int englishLettersCount = 26;
    const float kR = 0.038;
    const float kP = 0.065;
    int totalLetters = 0;
    int sumOfLetters = 0;

    int letters[englishLettersCount];
    fill(letters, letters + englishLettersCount, 0);

    int cipherLength = cipher.length();

    for (int i = 0; i < cipherLength; i++)
    {
        char letter = cipher[i];
        int orderOfLetter = (int)letter - 97;
        letters[orderOfLetter]++;
    }

    for(int letter : letters)
    {
        totalLetters = totalLetters + letter;
        sumOfLetters = sumOfLetters + (letter * (letter - 1));
    }

    float kO = (float)sumOfLetters / (totalLetters * (totalLetters - 1));

    float kappa = (kP - kR) / (kO - kR);

    return kappa;
}

// Function finds number of occurrences of given string in cipher
int countSequenceFrequencies(string &pat, string &txt)
{
    int patternLength = pat.length();
    int textLength = txt.length();
    int count = 0;
    
    for (int i = 0; i <= textLength - patternLength; i++)
    {
        int j;
        for (j = 0; j < patternLength; j++)
        {
            if (txt[i + j] != pat[j])
                break;
        }

        if (j == patternLength)
            count++;
    }

    return count;
}

// Function finds distances between occurrences of given string in cipher
vector<int> findDistances(vector<int> distances, string sequence, int position, string cipher)
{
    //cout << sequence << endl;
    //cout << cipher << endl;
    int sequenceFrequency = countSequenceFrequencies(sequence, cipher);
    int oldPosition = position;

    //cout << "First postition " << position << endl;

    for(int i = 0; i < sequenceFrequency - 1; i++)
    {
        position = cipher.find(sequence, position + 1);
        int distance = position - oldPosition;
        //cout << position << "-" << oldPosition << "=" << distance << endl;
        distances.push_back(distance);
        oldPosition = position;
    }

    return distances;
}

// Function finds all dividors of given number
vector<int> findDividors(vector<int> dividors, int number)
{
    for (int i = 1; i <= number; i++)
    {
        if (number % i == 0 && i != 1 && i != 2)
            dividors.push_back(i);
    }

    return dividors;
}

// Function finds most frequent dividor in given vector
int mostFrequentDividor(const vector<int>& dividors)
{
    map<int, int> counters;
    for(auto i: dividors)
        counters[i]++;

    int currentMax = 0;
    int argMax = 0;

    for(const auto & counter : counters)
    {
        //cout << counter.first << ":" << counter.second << endl;

        if (counter.second >= currentMax)
        {
            argMax = counter.first;
            currentMax = counter.second;
        }
    }

    for(const auto & counter : counters)
    {
        if (counter.second >= currentMax * 0.75 && counter.first >= argMax)
            argMax = counter.first;

    }

    return argMax;
}

// Kasiski test
int kasiskiTest(const string& cipher)
{
    int cipherLength = cipher.length();
    string sequence;

    vector<int> distances;
    vector<int> dividors;

    // We search all substrings of length 3 in first half of cipher
    for(int i = 0; i < cipherLength/2; i++)
    {
        sequence = cipher.substr(i, 3);
        string cipherCopy = cipher;
        string searchedCipher = cipherCopy.erase(0, i+3);

        if (searchedCipher.find(sequence) != string::npos)
            distances = findDistances(distances, sequence, i, cipher);
    }

    for (int distance : distances)
        dividors = findDividors(dividors, distance);

    int kasiskiResult = mostFrequentDividor(dividors);

    return kasiskiResult;
}

// Function finds length of cipher segment which contains every k-th letter
int findSegmentLength(int keyLength, int cipherLength, int i)
{
    int segmentLength;

    if (i < cipherLength % keyLength)
        segmentLength = cipherLength/keyLength + 1;
    else
        segmentLength = cipherLength/keyLength;

    return segmentLength;
}

// Function shifts every letter of cipher segment to the left
char* shiftLeft(char *textSegment, int segmentLength, int amount)
{
    for (int i = 0; i < segmentLength; i++)
    {
        if (textSegment[i] == 'a')
            textSegment[i] = char(textSegment[i] + 25);
        else
            textSegment[i] = char(textSegment[i] - 1);
    }

    return textSegment;
}

// Function finds correct letter on corresponding position of key
char findLetter(char *textSegment, float *englishLetterFrequency, int segmentLength)
{
    const int englishLettersCount = 26;
    char *shiftedTextSegment;
    int lettersCount[englishLettersCount];
    float chi;
    int minIndex = 0;
    float minChi = 0;
    float fi;

    for (int i = 0; i < englishLettersCount; i++)
    {
        // Shift cipher segment to the left
        if (i == 0)
            shiftedTextSegment = textSegment;
        else
            shiftedTextSegment = shiftLeft(textSegment, segmentLength, i);

        fill(lettersCount, lettersCount + englishLettersCount, 0);

        // Count letter occurrences in segment
        for (int j = 0; j < segmentLength; j++)
        {
            char letter = shiftedTextSegment[j];
            int orderOfLetter = (int)letter - 97;
            lettersCount[orderOfLetter]++;
        }

        chi = 0;

        // Find CHI
        for (int k = 0; k < englishLettersCount; k++)
        {
            fi = float(lettersCount[k]) / segmentLength;
            chi = chi + (pow((fi - englishLetterFrequency[k]), 2) / englishLetterFrequency[k]);
        }

        if(minChi == 0 || chi < minChi)
        {
            minChi = chi;
            minIndex = i;
        }

    }

    return char('a' + minIndex);
}

// Function finds key of Vigenere cipher
void findKey(const string& cipher, int keyLength)
{
    const int cipherLength = cipher.length();
    const int englishLettersCount = 26;
    //const float englishLetterFrequency[] = {0.0815, 0.0144, 0.0276, 0.0379, 0.1311, 0.0292, 0.0199, 0.0526, 0.0635, 0.0013, 0.0042, 0.0339, 0.0254, 0.0710, 0.0800, 0.0198, 0.0012, 0.0683, 0.0610, 0.1047, 0.0246, 0.0092, 0.0154, 0.0017, 0.0198, 0.0008};
    const float englishLetterFrequency[] = {0.082, 0.015, 0.028, 0.043, 0.13, 0.022, 0.02, 0.061, 0.07, 0.0015, 0.0077, 0.04, 0.024, 0.067, 0.075, 0.019, 0.00095, 0.06, 0.063, 0.091, 0.028, 0.0098, 0.024, 0.0015, 0.02, 0.00074};

    int segmentLength;
    int lettersCount[englishLettersCount];

    for (int i = 0; i < keyLength; i++)
    {
        segmentLength = findSegmentLength(keyLength, cipherLength, i);
        char *textSegment = new char[segmentLength];
        fill(lettersCount, lettersCount + englishLettersCount, 0);

        for (int j = 0; j < segmentLength; j++)
        {
            if (i + j * keyLength < cipherLength)
                textSegment[j] = cipher[i + j * keyLength];
        }

        char correctLetter = findLetter(textSegment, const_cast<float *>(englishLetterFrequency), segmentLength);
        cout << correctLetter;

        delete[] textSegment;
        textSegment = nullptr;
    }
}

// Funcia erase from cipher invalid characters
string processCipher(string cipher)
{
    int cipherLength = cipher.length();
    string newCipher = cipher;
    int toErase = 0;

    for(int i = 0; i < cipherLength; i++)
    {
        char letter = cipher[i];
        if (letter >= 'a' && letter <= 'z')
            newCipher[i - toErase] = letter;

        else if (letter >= 'A' && cipher[i] <= 'Z')
            newCipher[i - toErase] = char(letter + 32);

        else
            toErase++;
    }

    newCipher.erase(cipherLength - toErase);

    return newCipher;
}

int main() {

    string input;
    string cipher;

    while(getline(cin, input))
        cipher += input;

    cipher = processCipher(cipher);

    float friedman = friedmanTest(cipher);
    cout << friedman << ";";

    int kasiski = kasiskiTest(cipher);
    cout << kasiski << ";";

    int keyLength = kasiski;
    cout << keyLength << ";";

    findKey(cipher, keyLength);
    cout << endl;

    return 0;
}