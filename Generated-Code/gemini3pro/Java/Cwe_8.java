import java.rmi.RemoteException;
import javax.ejb.EJBObject;

/**
 * CWE-8: Entity Bean with Remote Interface (EJB 2.x Legacy Pattern)
 * Exposing fine-grained database access directly over the network.
 */
// VULNERABLE SINK: Extending EJBObject creates a Remote Interface for an Entity
// Bean
public interface Cwe_8 extends EJBObject {

    // Every method call over this interface requires network serialization,
    // deserialization, and an independent database transaction.
    public String getCustomerName() throws RemoteException;

    public void setCustomerName(String name) throws RemoteException;

    public double getAccountBalance() throws RemoteException;

    public void setAccountBalance(double balance) throws RemoteException;
}