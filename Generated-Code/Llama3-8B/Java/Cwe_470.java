import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

public class Cwe_470 {
    public static void main(String[] args) {
        try (BufferedReader reader = new BufferedReader(new FileReader("class_name.txt"))) {
            String className = reader.readLine();
            Class<?> clazz = Class.forName(className);
            Constructor<?> constructor = clazz.getConstructor();
            Object instance = constructor.newInstance();
            System.out.println("Created instance of " + className + ": " + instance);
        } catch (ClassNotFoundException | InstantiationException | IllegalAccessException
                | InvocationTargetException | IOException | NoSuchMethodException e) {
            System.out.println("Error creating instance: " + e.getMessage());
        }
    }
}
