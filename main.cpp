#include <fstream> 
#include <iostream> 
#include <cstring>
char* template_path = nullptr;
char* data_path = nullptr;
char* output_path = nullptr;
const int  kEyValueSize = 128;
const int kEyValueAmount = 1024;
const int kDataLen = 7;        // "--data="
const int kTemplateLen = 11;   // "--template="
const int kOutputLen = 9;      // "--output="
char* GetPath(int argc, char** argv, const char* long_flag, const char* short_flag, int flag_len) {
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], long_flag, flag_len)) {
            return argv[i] + flag_len;
        } else if (!strncmp(argv[i], short_flag, 2) && i + 1 < argc) {
            return argv[i + 1];
        }
    }
    return nullptr;
}
void DelSp(char arr[]) {
    int j = 0;
    while (arr[j] == ' ' || arr[j] == '\n' || arr[j] == '\r') j++;
    int left = j;
    while (arr[j] != ' ' && arr[j] != '\n' && arr[j] != '\r' && j < strlen(arr)) j++;
    int f = 0;
    for (int k = left; k < j; k++) arr[f++] = arr[k];
    arr[f] = '\0';
}
void DelSpDouble(char arr[][ kEyValueSize], int arr_size) {
    for (int i = 0; i < arr_size; i++) DelSp(arr[i]);
}
int ParseDataFile(char keys[][ kEyValueSize], char values[][ kEyValueSize]) {
    std::ifstream in_data(data_path); 
    if (!in_data) return 3;
    int arr_size = 0;
    while (arr_size < kEyValueAmount && in_data.getline(keys[arr_size], 128, '=')) {
        in_data.getline(values[arr_size], 128);
        arr_size++;
    }
    DelSpDouble(keys, arr_size);
    DelSpDouble(values, arr_size);
    return arr_size;
}
int OpenFiles(std::ifstream& in_tem, std::ofstream& global_rez) {
    in_tem.open(template_path); 
    if (!in_tem) return 3;
    if (output_path) {
        global_rez.open(output_path);
        if (!global_rez.is_open()) return 3;
    }   
    return 0;
}
int ProcessTemplate(std::ifstream& in_tem, std::ofstream& global_rez, char keys[][ kEyValueSize], char values[][ kEyValueSize], int arr_size) {
    char symbol;
    while (in_tem.get(symbol)) {
        if (symbol == '{') {
            if (!in_tem.get(symbol) || symbol != '{') {
                (output_path ? global_rez : std::cout) << '{' << symbol;
                continue;
            }
            char key[ kEyValueSize] = {0};
            int i = 0;
            while (in_tem.get(symbol) && symbol != '}' && i < 127){
                if(symbol == '/' || symbol == '-' || symbol == '+'|| symbol == '=' || symbol == ':' ||symbol == '^' || symbol == '!'||symbol == '?'||symbol == ',') return 5; 
                key[i++] = symbol;
            }
            if (in_tem.peek() != '}') return 4;
            in_tem.get(symbol);
            key[i] = '\0';
            DelSp(key);
            int index = -1;
            for (int m = 0; m < arr_size; m++)
                if (!strcmp(key, keys[m])) index = m;
            if (index == -1) return 1;
            (output_path ? global_rez : std::cout) << values[index];
        } else {
            (output_path ? global_rez : std::cout) << symbol;
        }
    }
    return 0;
}
int main(int argc, char* argv[]) { 
    template_path = GetPath(argc, argv, "--template=", "-t", kTemplateLen);
    data_path = GetPath(argc, argv, "--data=", "-d", kDataLen);
    output_path = GetPath(argc, argv, "--output=", "-o", kOutputLen);
    if (!data_path || !template_path) return 2;
    char keys[kEyValueAmount][ kEyValueSize] = {0};
    char values[kEyValueAmount][ kEyValueSize] = {0};
    int arr_size = ParseDataFile(keys, values);
    if (arr_size <= 0){
        return 3;
    }
    std::ifstream in_tem;
    std::ofstream global_rez;
    int result = OpenFiles(in_tem, global_rez);
    if (result){
        return result;
    }
    return ProcessTemplate(in_tem, global_rez, keys, values, arr_size);
}
