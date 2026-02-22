package chatgpt5.java;

/*
 * Secure Implementation - CWE-581
 *
 * Properly overrides equals() and hashCode() to compare Device objects
 * by their serialNumber. Demonstrates object comparison.
 */

public class cwe_581 {

    public static void main(String[] args) {

        Device device1 = new Device("ABC123", "Laptop");
        Device device2 = new Device("ABC123", "Laptop");
        Device device3 = new Device("XYZ789", "Tablet");

        System.out.println("device1.equals(device2): " + device1.equals(device2)); // true
        System.out.println("device1.equals(device3): " + device1.equals(device3)); // false

        System.out.println("device1.hashCode() == device2.hashCode(): " +
                (device1.hashCode() == device2.hashCode())); // true
    }
}

/*
 * Device class
 */
class Device {

    private final String serialNumber;
    private final String type;

    public Device(String serialNumber, String type) {
        this.serialNumber = serialNumber;
        this.type = type;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getType() {
        return type;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true; // same reference
        if (obj == null || getClass() != obj.getClass())
            return false; // null/type check
        Device other = (Device) obj;
        return serialNumber != null && serialNumber.equals(other.serialNumber);
    }

    @Override
    public int hashCode() {
        return serialNumber != null ? serialNumber.hashCode() : 0;
    }
}