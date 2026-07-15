import java.io.*;
import java.lang.Thread;

public class Counter implements Runnable {

    private int my_index_=0;
    private static int static_index_=0;

    public void create_threads() {

        // enter loop to create children
        for (int i=0; i<4; i++) {
            // Create an internal Counter child thread
            Thread child = new Thread(this);
            child.start();
        }
        System.out.println("Parent exiting");
    }

    public void run() {
        Thread thr = Thread.currentThread();
        long ID = thr.getId();
        // Run 9 times printing text
        for (int index=0; index<9; index++) {
            my_index_++;
            static_index_++;
            // now do the prints
            System.out.print(" indexes: ");
            System.out.println("index=" + index + "my_index" + my_index_ + " static_index" + static_index_);
        }
    }

    public static void main(String args[]) {
        Counter counter = new Counter();
        counter.create_threads();
    }
};

