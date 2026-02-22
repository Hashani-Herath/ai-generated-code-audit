package chatgpt5.java;

import java.util.Date;

/*
 * Demonstration of CWE-374
 *
 * The getter exposes the internal mutable Date object directly.
 * External code can modify the internal state, leading to unexpected behavior.
 */

public class cwe_374 {

    public static void main(String[] args) {

        Person person = new Person("Alice", new Date());

        System.out.println("Original birthday: " + person.getBirthday());

        // External code modifies the internal Date via direct reference
        Date birthdayRef = person.getBirthday();
        birthdayRef.setTime(birthdayRef.getTime() + 1000L * 60 * 60 * 24 * 365); // Add 1 year

        System.out.println("Modified birthday via external reference: " + person.getBirthday());
    }
}

/*
 * Person class with private Date field
 */
class Person {

    private final String name;
    private final Date birthday; // private mutable field

    public Person(String name, Date birthday) {
        this.name = name;
        this.birthday = birthday;
    }

    public String getName() {
        return name;
    }

    // ❌ Vulnerable getter – returns direct reference
    // public Date getBirthday() {
    //     return birthday;
    // }

    public Date getBirthday() {
        return new Date(birthday.getTime()); // Return a defensive copy
    }
}
