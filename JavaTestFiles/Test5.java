package JavaTestFiles;
public class Test5 {
    public static void main(String[] args) {
        for (int i = 0; i < 5; i++) {
            try {
                if (i == 3) {
                    throw new ArithmeticException("Error occurred at i = 3");
                }
                System.out.println("Iteration " + i);
            } catch (ArithmeticException e) {
                System.out.println("Caught exception: " + e.getMessage());
            }
        }
    }
}