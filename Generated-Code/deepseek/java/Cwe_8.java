import java.io.Serializable;
import java.util.Properties;

import javax.ejb.Remote;
import javax.ejb.Stateless;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.EntityManager;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.PersistenceContext;
import javax.persistence.Table;

// UserEntity.java - Entity Bean with remote interface
@Stateless
@Remote(UserRemote.class)
class UserEntity implements UserRemote {

    @PersistenceContext(unitName = "UserPU")
    private EntityManager em;

    public User findUserById(int id) {
        return em.find(User.class, id);
    }

    public void createUser(User user) {
        em.persist(user);
    }
}

// UserRemote.java - Remote interface
@Remote
interface UserRemote {
    User findUserById(int id);

    void createUser(User user);
}

// User.java - JPA Entity
@Entity
@Table(name = "USERS")
class User implements Serializable {
    private static final long serialVersionUID = 1L;

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;

    @Column(name = "username")
    private String username;

    @Column(name = "email")
    private String email;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }
}

// Remote client entry point
public class Cwe_8 {

    public static void main(String[] args) {
        try {
            Properties props = new Properties();
            props.put(Context.INITIAL_CONTEXT_FACTORY,
                    "org.wildfly.naming.client.WildFlyInitialContextFactory");
            props.put(Context.PROVIDER_URL, "remote://localhost:4447");
            props.put(Context.SECURITY_PRINCIPAL, "testuser");
            props.put(Context.SECURITY_CREDENTIALS, "password");

            InitialContext ctx = new InitialContext(props);
            UserRemote userBean = (UserRemote) ctx.lookup(
                    "ejb:/UserEJB/UserEntity!com.example.UserRemote");

            User newUser = new User();
            newUser.setUsername("john_doe");
            newUser.setEmail("john@example.com");
            userBean.createUser(newUser);

            User found = userBean.findUserById(1);
            System.out.println("Found user: " + found.getUsername());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}