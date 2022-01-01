/*
MIT License

Copyright (c) 2022 https://github.com/nmrr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

struct event
{
    float value;
    int32_t sample;
};

int32_t charArraytoInt32(char a, char b, char c, char d)
{
    union
    {
        int32_t value;
        char array[4];

    };

    array[0] = a;
    array[1] = b;
    array[2] = c;
    array[3] = d;

    return value;
}

int32_t charArraytoInt16(char a, char b)
{
    union
    {
        int16_t value;
        char array[2];

    };

    array[0] = a;
    array[1] = b;

    return value;
}

float charArraytoFloat(char a, char b, char c, char d)
{
    union
    {
        float value;
        char array[4];

    };

    array[0] = a;
    array[1] = b;
    array[2] = c;
    array[3] = d;

    return value;
}

bool checkParity(int * outputBitArray, int from, int to, int parity)
{
    int counter=0;
    for (int i=from; i<=to; i++)
    {
        if (outputBitArray[i] == 1) counter++;
    }
    if (counter%2 == parity) return true;
    else return false;
}

string getDayNameDCF(int day)
{
    switch (day)
    {
    case 1:
        return "Monday";
    case 2:
        return "Tuesday";
    case 3:
        return "Wednesday";
    case 4:
        return "Thursday";
    case 5:
        return "Friday";
    case 6:
        return "Saturday";
    case 7:
        return "Sunday";
    }

    return "Unknown";
}

void dcf77decode(int * outputBitArray, bool showArray)
{
    if (showArray == true)
    {
        for (int i=0; i<59; i++)
        {
            cout << "[" << i << "]" << "=" << outputBitArray[i];
            if (i!=58) cout << ", ";
        }
        cout << endl;
    }

    if (outputBitArray[0] == 0 && outputBitArray[20] == 1)
    {
        if (outputBitArray[17] ^ outputBitArray[18])
        {
            if (!checkParity(outputBitArray, 21, 27, outputBitArray[28]))
            {
                cout << "Bad parity for minutes" << endl;
                return;
            }

            int minute = outputBitArray[21] + 2*outputBitArray[22] + 4*outputBitArray[23] + 8*outputBitArray[24] + 10*outputBitArray[25] + 20*outputBitArray[26] + 40*outputBitArray[27];

            if (!checkParity(outputBitArray, 29, 34, outputBitArray[35]))
            {
                cout << "Bad parity for hours" << endl;
                return;
            }

            int hour = outputBitArray[29] + 2*outputBitArray[30] + 4*outputBitArray[31] + 8*outputBitArray[32] + 10*outputBitArray[33] + 20*outputBitArray[34];

            if (!checkParity(outputBitArray, 36, 57, outputBitArray[58]))
            {
                cout << "Bad parity for the date" << endl;
                return;
            }

            int day = outputBitArray[36] + 2*outputBitArray[37] + 4*outputBitArray[38] + 8*outputBitArray[39] + 10*outputBitArray[40] + 20*outputBitArray[41];
            int dayweek = outputBitArray[42] + 2*outputBitArray[43] + 4*outputBitArray[44];
            int month = outputBitArray[45] + 2*outputBitArray[46] + 4*outputBitArray[47] + 8*outputBitArray[48] + 10*outputBitArray[49];
            int year = 2000 + outputBitArray[50] + 2*outputBitArray[51] + 4*outputBitArray[52] + 8*outputBitArray[53] + 10*outputBitArray[54] + 20*outputBitArray[55] + 40*outputBitArray[56] + 80*outputBitArray[57];

            cout << year << "/" << (month < 10 ? "0" : "") << month << "/" << (day < 10 ? "0" : "") << day << " (" << getDayNameDCF(dayweek) << ") - " << (hour < 10 ? "0" : "")  << hour << ":" << (minute < 10 ? "0" : "") << minute << ((outputBitArray[17] == 0) ? " UTC+1" : " UTC+2") << (outputBitArray[19] == 1 ? "- A leap second will be added at the end of this hour" : "") << endl;
        }
        else cout << "Unable to read to determine time offset" << endl;
    }
    else cout << "Bit 0 and bit 20 must be equal to 1" << endl;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "<float32 wave file> <optional : \"1\" to display decoded values>" << endl;
        exit(1);
    }
    
    string fileName = string(argv[1]);
    
    bool showOutput = false;
    if (argc > 2) showOutput = (string(argv[2]) == "1" ? true : false);

    ifstream file (fileName, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        const size_t fileSize = file.tellg();

        if (fileSize > 48)
        {
            char * buffer = new char[fileSize];
            file.seekg (0, ios::beg);
            file.read (buffer, fileSize);

            int16_t format = charArraytoInt16(buffer[20], buffer[21]);
            int16_t channel = charArraytoInt16(buffer[22], buffer[23]);
            int32_t sampleRate = charArraytoInt32(buffer[24], buffer[25], buffer[26], buffer[27]);
            int32_t cksize = charArraytoInt32(buffer[40], buffer[41], buffer[42], buffer[43]);
            int32_t dwSampleLength = charArraytoInt32(buffer[44], buffer[45], buffer[46], buffer[47]);
            size_t dataPosition = fileSize - cksize*dwSampleLength;
            
            // DUMB FLOAT32 WAVE FILE DETECTOR
            if (buffer[0] == 'R' && buffer[1] == 'I' && buffer[2] == 'F' && buffer[3] == 'F' && buffer[8] == 'W' && buffer[9] == 'A' && buffer[10] == 'V' && buffer[11] == 'E' && buffer[12] == 'f' && buffer[13] == 'm' && buffer[14] == 't' && buffer[15] == ' ' && format == 3 && channel == 1 && sampleRate > 0 && dataPosition > 0 && cksize == 4)
            {
                float * arrayFloat = new float[dwSampleLength];
                uint32_t arrayFloatPosition = 0;

                for (size_t i=dataPosition; i<fileSize; i+=4)
                {
                    if (i + 4 <= fileSize)
                    {
                        arrayFloat[arrayFloatPosition] = charArraytoFloat(buffer[i], buffer[i+1], buffer[i+2], buffer[i+3]);
                        arrayFloatPosition++;
                    }
                }
                delete [] buffer;

                // RECTIFIER
                for (int32_t i=0; i<dwSampleLength; i++)
                {
                    if (arrayFloat[i] < 0) arrayFloat[i] *= -1;
                }

                // LOW-PASS FILTER
                int32_t lowPassSamples = sampleRate/480;
                if (lowPassSamples < 10) lowPassSamples = 10;
                
                bool end = false;
                for (int32_t i=0; i<dwSampleLength; i++)
                {
                    if (i+lowPassSamples >= dwSampleLength)
                    {
                        lowPassSamples = dwSampleLength - i;
                        end = true;
                    }

                    float average = 0;
                    for (int32_t j=i; j<i+lowPassSamples; j++) average += arrayFloat[j];

                    average /= lowPassSamples;
                    for (int32_t j=i; j<i+lowPassSamples; j++) arrayFloat[j] = average;

                    if (end == true) break;
                }

                // LIMITER
                for (int32_t i=0; i<dwSampleLength; i++)
                {
                    if (arrayFloat[i] > 1) arrayFloat[i] = 1;
                    else if (arrayFloat[i] < 0) arrayFloat[i] = 0;
                }

                // SCHMITT TRIGGER
                float minimum = arrayFloat[0];
                float maximum = arrayFloat[0];
                for (int32_t i=1; i<(sampleRate*2 < dwSampleLength ? sampleRate*2 : dwSampleLength); i++)
                {
                    if (arrayFloat[i] < minimum) minimum = arrayFloat[i];
                    if (arrayFloat[i] > maximum) maximum = arrayFloat[i];
                }

                float thresholdMax = ((maximum+minimum)/2) * 1.25;
                float thresholdMin = ((maximum+minimum)/2) * 0.75;

                uint8_t schmittTrigger = ((maximum+minimum)/2 > arrayFloat[0] ? 1 : 0);
                uint32_t eventNumber = 0;
                uint32_t samplePosition = 0;

                vector<event> vectorEvent;

                for (int32_t i=1; i<dwSampleLength; i++)
                {
                    if (arrayFloat[i] > thresholdMax && schmittTrigger == 0)
                    {
                        schmittTrigger = 1;
                        if (eventNumber > 2)
                        {
                            event newEvent;
                            newEvent.sample = i-samplePosition;
                            newEvent.value = 1;

                            if (newEvent.sample >= sampleRate/25)
                            {
                                thresholdMin = ((maximum+minimum)/2) * 0.75;
                                vectorEvent.push_back(newEvent);
                                samplePosition = i;
                                maximum = arrayFloat[i];
                            }
                        }

                        eventNumber++;
                    }
                    else if (arrayFloat[i] < thresholdMin && schmittTrigger == 1)
                    {
                        schmittTrigger = 0;
                        if (eventNumber > 2)
                        {
                            event newEvent;
                            newEvent.sample = i-samplePosition;
                            newEvent.value = 0;

                            if (newEvent.sample >= sampleRate/25)
                            {
                                thresholdMax = ((maximum+minimum)/2) * 1.25;
                                vectorEvent.push_back(newEvent);
                                samplePosition = i;
                                minimum = arrayFloat[i];
                            }
                        }

                        eventNumber++;
                    }

                    if (schmittTrigger == 1)
                    {
                        if (arrayFloat[i] > maximum) maximum = arrayFloat[i];
                    }
                    else
                    {
                        if (arrayFloat[i] < minimum) minimum = arrayFloat[i];
                    }
                }
                
                // EVENT DECODER
                int dcf77Array[59];
                uint32_t dcf77Position = 0;
                bool dcf77ReadData = false;

                for (uint32_t i=0; i<vectorEvent.size(); i++)
                {
                    double time = double(vectorEvent[i].sample)/double(sampleRate);

                    if (vectorEvent[i].value == 0 && time >= 1.6 && time <= 2.1)
                    {
                        dcf77Position = 0;
                        dcf77ReadData = true;
                        cout << "New minute detected" << endl;
                    }
                    else if (dcf77ReadData == true && vectorEvent[i].value == 1)
                    {
                        if (time >= 0.08 && time <= 0.12) dcf77Array[dcf77Position++] = 0;
                        else if (time >= 0.18 && time <= 0.22) dcf77Array[dcf77Position++] = 1;
                        else
                        {
                            cout << "Error during reading data (" << dcf77Position << ")" << endl;
                            dcf77ReadData = false;
                        }

                        if (dcf77Position == 59)
                        {
                            dcf77decode(dcf77Array, showOutput);
                            cout << endl;
                            dcf77ReadData = false;
                        }
                    }
                }

                delete [] arrayFloat;
            }
            else
            {
                cerr << "Not a float32 single channel wave file" << endl;
                delete [] buffer;
            }
        }
        else
        {
            cerr << "Not a wave file" << endl;
        }


        file.close();
    }
    else
    {
        cout << "Cannot open the file" << endl;
    }

    return 0;
}
