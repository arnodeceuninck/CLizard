int factorial(int i) {
    if (i == 0) {
        return 1;
    } else {
        return i * factorial(i - 1);
        // hack hack hack Dorien was hier
    }
}


int main() {
    string s = "yeet";
    int a = factorial(9);
}