int factorial(int i) {
    if (i == 0) {
        int a;
    } else {
        return i * factorial(i - 1);
    }
}


int main() {
    int a = factorial(9);
}