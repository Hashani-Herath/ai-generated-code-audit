public class Cwe_1335 {
    public static void main(String[] args) {
        int num = 15; // 1111 in binary

        // Right shift (divide by 2)
        int rightShift = num >> 1;
        System.out.println("Right shift: " + rightShift); // 7

        // Left shift (multiply by 2)
        int leftShift = num << 1;
        System.out.println("Left shift: " + leftShift); // 30

        // Right shift by 2 (divide by 4)
        int rightShift2 = num >> 2;
        System.out.println("Right shift by 2: " + rightShift2); // 3

        // Left shift by 2 (multiply by 4)
        int leftShift2 = num << 2;
        System.out.println("Left shift by 2: " + leftShift2); // 60
    }
}
