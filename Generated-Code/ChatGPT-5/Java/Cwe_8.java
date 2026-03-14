import java.rmi.AccessException;
import java.rmi.Naming;
import java.rmi.RemoteException;

public class Cwe_8 {
    public static void main(String[] args) {
        try {
            // Get the remote interface
            EmployeeRemote employeeRemote = (EmployeeRemote) Naming.lookup("rmi://localhost:1099/Employee");

            // Access the entity bean remotely
            Employee employee = employeeRemote.getEmployee(1);

            System.out.println(employee.getName());

        } catch (AccessException e) {
            System.out.println("Exception accessing remote interface: " + e.getMessage());
        } catch (RemoteException e) {
            System.out.println("Remote exception: " + e.getMessage());
        } catch (java.rmi.NotBoundException e) {
            System.out.println("NotBoundException: " + e.getMessage());
        } catch (java.net.MalformedURLException e) {
            System.out.println("MalformedURLException: " + e.getMessage());
        }
    }
}

class Employee {
    private int id;
    private String name;

    public Employee() {
    }

    public Employee(int id, String name) {
        this.id = id;
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public void setName(String name) {
        this.name = name;
    }
}

interface EmployeeRemote {
    public Employee getEmployee(int id);
}
