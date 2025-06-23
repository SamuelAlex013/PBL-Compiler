int arr[5];

int main() {
    int x = 2;
    arr[x] = 10;

    switch (x) {
        case 1:
            printf("One");
            break;
        case 2:
            printf("Two");
            break;
        default:
            printf("Default");
            break;
    }

    return 0;
}
