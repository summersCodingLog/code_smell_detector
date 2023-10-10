#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;
//Encapsulate functions as class
class functions{
public:
    vector<string> body;
    //get lines within each function
    void pushBack(const string& line){
        if(!line.empty()){
            body.push_back(line);
        }
    }
    //get function name
    string getName() const {
        string first = body[0];
        first = first.substr(first.find(' ') + 1);
        size_t openParenIndex = first.find('(');
        if (openParenIndex != string::npos) {
            return first.substr(0, openParenIndex);
        }
        size_t openBraceIndex = first.find('{');
        if (openBraceIndex != string::npos) {
            return first.substr(0, openBraceIndex);
        }
        return first;
    }
};

void checkLongMethod(const vector<functions>& f) {
    int num=0;
    for (const functions& item: f){
        if(item.body.size() > 15){
            num++;
            cout<<"The "<<item.getName()<<" function is a Long Function. It contains "
                <<item.body.size()<<" lines of code."<<endl;
        }
    }
    if(num==0)
        cout<<"No function is a Long Function."<<endl;
}

void checkLongParameters(const vector<functions>& ms) {
    int num=0;
    for (functions item: ms){
        string temp=item.body[0];
        int countNum=(int)count(temp.begin(),temp.end(),',')+1;
        if(countNum>=4){
            num++;
            cout<<item.getName()<<" has a Long Parameter List. Its parameter list contains "<<
                countNum<<" parameters."<<endl;
        }
    }
    if(num==0)
        cout<<"No function has a long parameter list."<<endl;
}

double getJaccard(const functions& f1,const functions& f2){
    set<char> s1,s2;
    for (const string &item: f1.body)
        for (const char &c: item)
            s1.insert(c);

    for (const string &item: f2.body)
        for (const char &c: item)
            s2.insert(c);

    set<int> intersection;
    set<int> convergence;
    set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(intersection, intersection.begin()));
    set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(convergence, convergence.begin()));
    return (double)intersection.size()/(double)convergence.size();
}

void checkDuplicated(const vector<functions>& ms){
    int num=0;
    for(int i=0;i<ms.size();i++)
        for(int j=i+1;j<ms.size();j++){
            double score= getJaccard(ms[i],ms[j]);
            if(score>0.75){
                num++;
                cout<<ms[i].getName()<<" and "<<ms[j].getName()<<" are duplicated. The Jaccard similarity is "<<score<<"."<<endl;
            }
        }
    if(num==0)
        cout<<"No functions are duplicated.";
}

bool isValid(const vector<char>&stack,char c){
    if(stack.empty()) return true;
    char tmp=stack[stack.size()-1];
    if(tmp=='\''||tmp=='"') return false;
    return true;
}

vector<functions> split(const vector<string>& lines){
    vector<functions> result;
    vector<char> stack;
    functions tmp;
    for (const string &item: lines){
        bool trans=false;
        for(char c:item){
            if(trans){
                trans= false;
                continue;
            }
            if(c=='\\')trans=true;
            if(c=='\''||c=='"'){
                if(stack[stack.size()-1]==c) stack.pop_back();
                else stack.push_back(c);
            }
            if(c=='{'&& isValid(stack,'{')){
                if(stack.empty()){
                    tmp=functions();
                }
                stack.push_back('{');
            }
            if(c=='}'&& stack[stack.size()-1]=='{'){
                stack.pop_back();
                if(stack.empty()){
                    result.push_back(tmp);
                }
            }
        }
        tmp.pushBack(item);
    }
    return result;
}

vector<string> readFile(const string& filename) {
    vector<string> lines;
    ifstream file(filename);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    } else {
        cout << "Unable to open file: " << filename << endl;
    }

    return lines;
}

void displayMethods(const vector<functions>& result) {
    cout << "Welcome to Code Smell Detection! The file you input contains the following methods/functions:" << endl;
    for (int i = 0; i < result.size(); i++)
        if (i == result.size() - 1)
            cout << result[i].getName() << ". " << endl;
        else
            cout << result[i].getName() << ", ";
}

void processMenuChoice(const string& input, const vector<functions>& result, int& flag) {
    if (input == "1") {
        checkLongMethod(result);
    } else if (input == "2") {
        checkLongParameters(result);
    } else if (input == "3") {
        checkDuplicated(result);
    } else if (input == "4") {
        flag = 0;
    } else {
        cout << "Invalid choice. Please try again." << endl;
    }
}

void displayMenu() {
    cout << "\nPlease choose what you want to do now:" << endl;
    cout << "1. Long Method/Function Detection" << endl;
    cout << "2. Long Parameter List Detection" << endl;
    cout << "3. Duplicated Code Detection" << endl;
    cout << "4. Quit" << endl;
}

int main() {
    string filename;
    cout << "Please input file parameter:" << endl;
    cin >> filename;
    vector<string> lines = readFile(filename);
    if (lines.empty()) {
        return 0;
    }

    vector<functions> result = split(lines);
    displayMethods(result);

    int flag = 1;
    while (flag) {
        displayMenu();
        string input;
        cin >> input;
        processMenuChoice(input, result, flag);
    }
    return 0;
}
