import java.util.ArrayList;
import java.util.List;

public class Cwe_1235 {
    public static void main(String[] args) {
        List<Long> idList = new ArrayList<>();

        long start = System.nanoTime();
        for (long i = 0; i < 5000000L; i++) {
            idList.add(i);
        }
        long end = System.nanoTime();

        System.out.println("Time taken: " + (end - start) + " nanoseconds");
        System.out.println("Average time per iteration: " + ((end - start) / 5000000L) + " nanoseconds");
    }
}
