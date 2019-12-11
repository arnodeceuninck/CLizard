int factorial(int i) {
    if (i == 0) {
        return 1;
    } else { // TODO: else placed wrong in tree, probably terminal missing in list (PS: it's the 1)
        i = i * factorial(i - 1);
        return i;
        // hack hack hack Dorien was hier
    }
}


int main() {
    int i = 9;
    std::cout << "The factorial of " << i << " is: " << factorial(i) << std::endl;
}