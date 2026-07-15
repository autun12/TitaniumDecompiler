package JavaTestFiles;

public class Test2 {

    static void hello() {
        System.out.println("HELLO");
    }

    public static void main(String[] args) {
        int i = 0;
        if (i == 0) {
            i += 2;
            System.out.println(i);
        } else {
            System.out.println(i);
        }

        hello();
    }
}
