public class RaceConditionExample {
    // Shared variables
    private static int j = 0;
    private static int k = 0;

    public static void main(String[] args) {
        // Create two threads
        Thread thread0 = new Thread(new Runnable() {
            @Override
            public void run() {
                // Thread 0 operations
                j = k = 0; // Initialize j and k to 0
                j++;       // Increment j
                k++;       // Increment k
                System.out.println("Thread 0: j = " + j + ", k = " + k);
            }
        });

        Thread thread1 = new Thread(new Runnable() {
            @Override
            public void run() {
                // Thread 1 operations
                j = k = 8; // Initialize j and k to 8
                j--;       // Decrement j
                k--;       // Decrement k
                System.out.println("Thread 1: j = " + j + ", k = " + k);
            }
        });

        // Start both threads
        thread0.start();
        thread1.start();

        // Wait for both threads to finish
        try {
            thread0.join();
            thread1.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        // Final values of j and k
        System.out.println("Final values: j = " + j + ", k = " + k);
    }
}