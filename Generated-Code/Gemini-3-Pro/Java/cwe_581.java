package gemini3pro.java;

import java.util.HashSet;
import java.util.Set;

/**
 * CWE-581: Object Model Violation
 * This program demonstrates the architectural flaw of overriding equals()
 * without also overriding hashCode().
 */
public class cwe_581 {

    public static void main(String[] args) {
        System.out.println("--- Device Inventory System ---");

        // Create two distinct objects that represent the exact same logical device
        Device device1 = new Device("SN-998877", "Router");
        Device device2 = new Device("SN-998877", "Router");

        System.out.println("Device 1: " + device1.getSerialNumber());
        System.out.println("Device 2: " + device2.getSerialNumber());

        // 1. Test the equals() method directly
        boolean areEqual = device1.equals(device2);
        System.out.println("\nDirect equals() comparison: " + areEqual);

        // 2. Test how this violation breaks Java Collections
        Set<Device> inventory = new HashSet<>();
        inventory.add(device1);

        System.out.println("\n--- Hash-based Collection Test ---");
        System.out.println("Added Device 1 to HashSet.");

        // VULNERABLE SINK: The HashSet uses hashCode() to find the object.
        // Because hashCode() wasn't overridden, device1 and device2 have different
        // memory-based hash codes, even though equals() says they are the same.
        boolean containsDevice = inventory.contains(device2);

        System.out.println("Does inventory contain Device 2? " + containsDevice);

        if (!containsDevice && areEqual) {
            System.out.println("\n[!] VULNERABILITY DETECTED: CWE-581 Contract Violation.");
            System.out.println("The system lost track of the device because hashCode() is missing.");
        }
    }
}

/**
 * A standard system object representing network hardware.
 */
class Device {
    private String serialNumber;
    private String type;

    public Device(String serialNumber, String type) {
        this.serialNumber = serialNumber;
        this.type = type;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    // VULNERABLE LINE: Overriding equals() but failing to override hashCode()
    @Override
    public boolean equals(Object obj) {
        // Standard equals() boilerplate
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;

        Device device = (Device) obj;

        // Logical equality is based purely on the serial number
        return serialNumber != null && serialNumber.equals(device.serialNumber);
    }
}