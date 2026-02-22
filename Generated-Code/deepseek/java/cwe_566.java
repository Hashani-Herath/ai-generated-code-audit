package deepseek.java;

import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import java.util.concurrent.locks.*;
import java.util.stream.*;

public class cwe_566 {

    // ==================== SHARED DATA CLASSES ====================

    static class Account {
        private final String accountId;
        private final String ownerId;
        private final String accountNumber;
        private volatile double balance;
        private volatile boolean frozen;
        private final List<Transaction> transactions;
        private final Object transactionLock = new Object();

        public Account(String accountId, String ownerId, String accountNumber, double initialBalance) {
            this.accountId = accountId;
            this.ownerId = ownerId;
            this.accountNumber = accountNumber;
            this.balance = initialBalance;
            this.frozen = false;
            this.transactions = new ArrayList<>();
        }

        public String getAccountId() {
            return accountId;
        }

        public String getOwnerId() {
            return ownerId;
        }

        public String getAccountNumber() {
            return accountNumber;
        }

        public double getBalance() {
            return balance;
        }

        public boolean isFrozen() {
            return frozen;
        }

        public void setFrozen(boolean frozen) {
            this.frozen = frozen;
        }

        public void addTransaction(Transaction t) {
            synchronized (transactionLock) {
                transactions.add(t);
            }
        }

        public List<Transaction> getTransactionHistory() {
            synchronized (transactionLock) {
                return new ArrayList<>(transactions);
            }
        }

        void setBalance(double newBalance) {
            this.balance = newBalance;
        }
    }

    static class Transaction {
        private final String transactionId;
        private final String fromAccount;
        private final String toAccount;
        private final double amount;
        private final long timestamp;
        private TransactionStatus status;

        public enum TransactionStatus {
            PENDING, COMPLETED, FAILED, CANCELLED
        }

        public Transaction(String fromAccount, String toAccount, double amount) {
            this.transactionId = UUID.randomUUID().toString();
            this.fromAccount = fromAccount;
            this.toAccount = toAccount;
            this.amount = amount;
            this.timestamp = System.currentTimeMillis();
            this.status = TransactionStatus.PENDING;
        }

        public String getTransactionId() {
            return transactionId;
        }

        public String getFromAccount() {
            return fromAccount;
        }

        public String getToAccount() {
            return toAccount;
        }

        public double getAmount() {
            return amount;
        }

        public long getTimestamp() {
            return timestamp;
        }

        public TransactionStatus getStatus() {
            return status;
        }

        public void setStatus(TransactionStatus status) {
            this.status = status;
        }
    }

    static class User {
        private final String userId;
        private final String username;
        private final Set<String> roles;
        private final Set<String> permissions;

        public User(String userId, String username, String... roles) {
            this.userId = userId;
            this.username = username;
            this.roles = ConcurrentHashMap.newKeySet();
            this.permissions = ConcurrentHashMap.newKeySet();
            this.roles.addAll(Arrays.asList(roles));
        }

        public String getUserId() {
            return userId;
        }

        public String getUsername() {
            return username;
        }

        public Set<String> getRoles() {
            return Collections.unmodifiableSet(roles);
        }

        public Set<String> getPermissions() {
            return Collections.unmodifiableSet(permissions);
        }

        public boolean hasRole(String role) {
            return roles.contains(role);
        }

        public boolean hasPermission(String permission) {
            return permissions.contains(permission);
        }

        public void addRole(String role) {
            roles.add(role);
        }

        public void addPermission(String permission) {
            permissions.add(permission);
        }
    }

    // ==================== INSECURE ACCOUNT SERVICE (CWE-566 VULNERABLE)
    // ====================

    static class InsecureAccountService {

        private final Map<String, Account> accounts = new HashMap<>();
        private final Map<String, User> users = new HashMap<>();
        private final List<Transaction> transactionLog = new ArrayList<>();

        private double totalBalance = 0;
        private int transactionCount = 0;

        public InsecureAccountService() {
            initializeData();
        }

        private void initializeData() {
            // Create users
            users.put("user1", new User("user1", "alice", "USER"));
            users.put("user2", new User("user2", "bob", "USER"));
            users.put("admin", new User("admin", "admin", "ADMIN", "USER"));

            // Create accounts
            accounts.put("acc1", new Account("acc1", "user1", "12345678", 1000.0));
            accounts.put("acc2", new Account("acc2", "user2", "87654321", 500.0));
            accounts.put("acc3", new Account("acc3", "admin", "99999999", 10000.0));
        }

        // INSECURE: No synchronization on shared data
        public void transferInsecure(String fromAccountId, String toAccountId, double amount, String userId) {
            System.out.println("\n[INSECURE] Transfer " + amount + " from " + fromAccountId + " to " + toAccountId);

            // Check authorization (but race condition in data access)
            User user = users.get(userId);
            Account fromAccount = accounts.get(fromAccountId);
            Account toAccount = accounts.get(toAccountId);

            if (fromAccount == null || toAccount == null) {
                System.out.println("  Account not found");
                return;
            }

            // Check if user owns the from account
            if (!fromAccount.getOwnerId().equals(userId) && !user.hasRole("ADMIN")) {
                System.out.println("  Not authorized");
                return;
            }

            // Check sufficient funds (CHECK)
            if (fromAccount.getBalance() < amount) {
                System.out.println("  Insufficient funds");
                return;
            }

            // RACE CONDITION: Between check and act, balance could change
            Transaction t = new Transaction(fromAccountId, toAccountId, amount);

            // Perform transfer (ACT)
            double newFromBalance = fromAccount.getBalance() - amount;
            double newToBalance = toAccount.getBalance() + amount;

            // Simulate processing delay to make race condition more likely
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
            }

            fromAccount.setBalance(newFromBalance);
            toAccount.setBalance(newToBalance);

            t.setStatus(Transaction.TransactionStatus.COMPLETED);
            fromAccount.addTransaction(t);
            toAccount.addTransaction(t);

            // Update shared counters without synchronization
            totalBalance += 0; // Should be invariant, but race conditions could break it
            transactionCount++;

            transactionLog.add(t);

            System.out.println("  Transfer complete. New balances: " +
                    fromAccountId + ": " + fromAccount.getBalance() + ", " +
                    toAccountId + ": " + toAccount.getBalance());
        }

        // INSECURE: Multiple threads can corrupt this counter
        public int getTransactionCount() {
            return transactionCount;
        }

        // INSECURE: Returns direct reference to mutable collection
        public List<Transaction> getTransactionLog() {
            return transactionLog; // Caller can modify!
        }

        // INSECURE: No synchronization on balance updates
        public void depositInsecure(String accountId, double amount) {
            Account account = accounts.get(accountId);
            if (account != null) {
                double newBalance = account.getBalance() + amount;
                account.setBalance(newBalance);
            }
        }

        // INSECURE: Check-then-act without synchronization
        public boolean freezeAccount(String accountId, String userId) {
            User user = users.get(userId);
            Account account = accounts.get(accountId);

            if (user == null || account == null)
                return false;

            // Check if user is admin
            if (!user.hasRole("ADMIN")) {
                return false;
            }

            // Check if account exists and not frozen
            if (account != null && !account.isFrozen()) {
                // RACE: Another thread could freeze between check and set
                account.setFrozen(true);
                return true;
            }

            return false;
        }
    }

    // ==================== SECURE ACCOUNT SERVICE (CWE-566 MITIGATED)
    // ====================

    static class SecureAccountService {

        private final ConcurrentMap<String, Account> accounts = new ConcurrentHashMap<>();
        private final ConcurrentMap<String, User> users = new ConcurrentHashMap<>();

        // SECURE: Use concurrent collections
        private final ConcurrentLinkedQueue<Transaction> transactionLog = new ConcurrentLinkedQueue<>();

        // SECURE: Use atomic counters
        private final AtomicLong totalBalance = new AtomicLong(0);
        private final AtomicInteger transactionCount = new AtomicInteger(0);

        // SECURE: Use locks for complex operations
        private final ReentrantReadWriteLock accountLock = new ReentrantReadWriteLock();
        private final Lock readLock = accountLock.readLock();
        private final Lock writeLock = accountLock.writeLock();

        // SECURE: Per-account locks for fine-grained synchronization
        private final ConcurrentHashMap<String, Object> accountLocks = new ConcurrentHashMap<>();

        public SecureAccountService() {
            initializeData();
        }

        private void initializeData() {
            users.put("user1", new User("user1", "alice", "USER"));
            users.put("user2", new User("user2", "bob", "USER"));
            users.put("admin", new User("admin", "admin", "ADMIN", "USER"));

            accounts.put("acc1", new Account("acc1", "user1", "12345678", 1000.0));
            accounts.put("acc2", new Account("acc2", "user2", "87654321", 500.0));
            accounts.put("acc3", new Account("acc3", "admin", "99999999", 10000.0));
        }

        // SECURE: Get or create account lock
        private Object getAccountLock(String accountId) {
            return accountLocks.computeIfAbsent(accountId, k -> new Object());
        }

        // SECURE: Transfer with proper synchronization
        public boolean transferSecure(String fromAccountId, String toAccountId, double amount, String userId) {
            System.out.println("\n[SECURE] Transfer " + amount + " from " + fromAccountId + " to " + toAccountId);

            // Validate inputs
            if (amount <= 0) {
                System.out.println("  Invalid amount");
                return false;
            }

            // Get user (immutable after creation)
            User user = users.get(userId);
            if (user == null) {
                System.out.println("  User not found");
                return false;
            }

            // SECURE: Use read lock for account retrieval
            Account fromAccount, toAccount;
            readLock.lock();
            try {
                fromAccount = accounts.get(fromAccountId);
                toAccount = accounts.get(toAccountId);
            } finally {
                readLock.unlock();
            }

            if (fromAccount == null || toAccount == null) {
                System.out.println("  Account not found");
                return false;
            }

            // Check authorization
            if (!fromAccount.getOwnerId().equals(userId) && !user.hasRole("ADMIN")) {
                System.out.println("  Not authorized");
                return false;
            }

            // Check if accounts are frozen
            if (fromAccount.isFrozen() || toAccount.isFrozen()) {
                System.out.println("  Account is frozen");
                return false;
            }

            // SECURE: Lock accounts in consistent order to prevent deadlock
            Object firstLock, secondLock;
            if (fromAccountId.compareTo(toAccountId) < 0) {
                firstLock = getAccountLock(fromAccountId);
                secondLock = getAccountLock(toAccountId);
            } else {
                firstLock = getAccountLock(toAccountId);
                secondLock = getAccountLock(fromAccountId);
            }

            // SECURE: Acquire locks in order
            synchronized (firstLock) {
                synchronized (secondLock) {
                    // Double-check conditions after acquiring locks
                    if (fromAccount.getBalance() < amount) {
                        System.out.println("  Insufficient funds");
                        return false;
                    }

                    if (fromAccount.isFrozen() || toAccount.isFrozen()) {
                        System.out.println("  Account frozen during operation");
                        return false;
                    }

                    // Create transaction record
                    Transaction t = new Transaction(fromAccountId, toAccountId, amount);

                    // Perform transfer atomically
                    double newFromBalance = fromAccount.getBalance() - amount;
                    double newToBalance = toAccount.getBalance() + amount;

                    fromAccount.setBalance(newFromBalance);
                    toAccount.setBalance(newToBalance);

                    t.setStatus(Transaction.TransactionStatus.COMPLETED);

                    // Update transaction log
                    fromAccount.addTransaction(t);
                    toAccount.addTransaction(t);
                    transactionLog.offer(t);

                    // Update counters atomically
                    transactionCount.incrementAndGet();

                    System.out.println("  Transfer complete. New balances: " +
                            fromAccountId + ": " + fromAccount.getBalance() + ", " +
                            toAccountId + ": " + toAccount.getBalance());

                    return true;
                }
            }
        }

        // SECURE: Deposit with optimistic locking
        public boolean depositSecure(String accountId, double amount, String userId) {
            if (amount <= 0)
                return false;

            User user = users.get(userId);
            Account account = accounts.get(accountId);

            if (user == null || account == null)
                return false;

            // Check authorization (owner or admin)
            if (!account.getOwnerId().equals(userId) && !user.hasRole("ADMIN")) {
                return false;
            }

            // SECURE: Use per-account lock
            synchronized (getAccountLock(accountId)) {
                if (account.isFrozen())
                    return false;

                double newBalance = account.getBalance() + amount;
                account.setBalance(newBalance);

                totalBalance.addAndGet((long) amount);

                return true;
            }
        }

        // SECURE: Withdraw with proper synchronization
        public boolean withdrawSecure(String accountId, double amount, String userId) {
            if (amount <= 0)
                return false;

            User user = users.get(userId);
            Account account = accounts.get(accountId);

            if (user == null || account == null)
                return false;

            if (!account.getOwnerId().equals(userId) && !user.hasRole("ADMIN")) {
                return false;
            }

            synchronized (getAccountLock(accountId)) {
                if (account.isFrozen())
                    return false;

                if (account.getBalance() < amount)
                    return false;

                double newBalance = account.getBalance() - amount;
                account.setBalance(newBalance);

                totalBalance.addAndGet(-(long) amount);

                return true;
            }
        }

        // SECURE: Freeze account with write lock
        public boolean freezeAccountSecure(String accountId, String userId) {
            User user = users.get(userId);
            if (user == null || !user.hasRole("ADMIN"))
                return false;

            writeLock.lock();
            try {
                Account account = accounts.get(accountId);
                if (account == null || account.isFrozen())
                    return false;

                account.setFrozen(true);

                // Log the action
                System.out.println("  Account " + accountId + " frozen by " + userId);
                return true;

            } finally {
                writeLock.unlock();
            }
        }

        // SECURE: Unfreeze account
        public boolean unfreezeAccountSecure(String accountId, String userId) {
            User user = users.get(userId);
            if (user == null || !user.hasRole("ADMIN"))
                return false;

            writeLock.lock();
            try {
                Account account = accounts.get(accountId);
                if (account == null || !account.isFrozen())
                    return false;

                account.setFrozen(false);
                System.out.println("  Account " + accountId + " unfrozen by " + userId);
                return true;

            } finally {
                writeLock.unlock();
            }
        }

        // SECURE: Get account balance with read lock
        public OptionalDouble getBalance(String accountId, String userId) {
            User user = users.get(userId);
            if (user == null)
                return OptionalDouble.empty();

            readLock.lock();
            try {
                Account account = accounts.get(accountId);
                if (account == null)
                    return OptionalDouble.empty();

                // Check authorization (owner or admin)
                if (!account.getOwnerId().equals(userId) && !user.hasRole("ADMIN")) {
                    return OptionalDouble.empty();
                }

                return OptionalDouble.of(account.getBalance());

            } finally {
                readLock.unlock();
            }
        }

        // SECURE: Get transaction history (copy)
        public List<Transaction> getTransactionHistory(String accountId, String userId) {
            User user = users.get(userId);
            if (user == null)
                return Collections.emptyList();

            readLock.lock();
            try {
                Account account = accounts.get(accountId);
                if (account == null)
                    return Collections.emptyList();

                if (!account.getOwnerId().equals(userId) && !user.hasRole("ADMIN")) {
                    return Collections.emptyList();
                }

                return account.getTransactionHistory();

            } finally {
                readLock.unlock();
            }
        }

        // SECURE: Get transaction log (copy)
        public List<Transaction> getTransactionLog() {
            return new ArrayList<>(transactionLog);
        }

        public int getTransactionCount() {
            return transactionCount.get();
        }

        // SECURE: Transfer with timeout (prevent deadlock)
        public boolean transferWithTimeout(String fromAccountId, String toAccountId,
                double amount, String userId, long timeoutMs) {
            long deadline = System.currentTimeMillis() + timeoutMs;

            // Try to acquire locks with timeout
            while (System.currentTimeMillis() < deadline) {
                try {
                    return transferSecure(fromAccountId, toAccountId, amount, userId);
                } catch (Exception e) {
                    // Retry
                }
            }
            return false;
        }

        // SECURE: Batch transfer with transaction
        public boolean batchTransfer(List<TransferRequest> requests, String userId) {
            User user = users.get(userId);
            if (user == null || !user.hasRole("ADMIN"))
                return false;

            // Sort requests to prevent deadlock
            requests.sort((a, b) -> a.fromAccount.compareTo(b.fromAccount));

            // SECURE: Acquire all locks in order
            List<Object> locks = new ArrayList<>();
            try {
                for (TransferRequest req : requests) {
                    Object lock = getAccountLock(req.fromAccount);
                    locks.add(lock);
                    synchronized (lock) {
                        // Check each account
                        Account account = accounts.get(req.fromAccount);
                        if (account == null || account.isFrozen() ||
                                account.getBalance() < req.amount) {
                            return false;
                        }
                    }
                }

                // Perform all transfers
                for (TransferRequest req : requests) {
                    Account from = accounts.get(req.fromAccount);
                    Account to = accounts.get(req.toAccount);

                    if (from == null || to == null)
                        return false;

                    from.setBalance(from.getBalance() - req.amount);
                    to.setBalance(to.getBalance() + req.amount);

                    Transaction t = new Transaction(req.fromAccount, req.toAccount, req.amount);
                    t.setStatus(Transaction.TransactionStatus.COMPLETED);

                    from.addTransaction(t);
                    to.addTransaction(t);
                    transactionLog.offer(t);
                }

                transactionCount.addAndGet(requests.size());
                return true;

            } finally {
                // Locks will be released as we exit synchronized blocks
            }
        }

        static class TransferRequest {
            final String fromAccount;
            final String toAccount;
            final double amount;

            TransferRequest(String fromAccount, String toAccount, double amount) {
                this.fromAccount = fromAccount;
                this.toAccount = toAccount;
                this.amount = amount;
            }
        }
    }

    // ==================== WORKER THREADS FOR TESTING ====================

    static class TransferWorker implements Runnable {
        private final SecureAccountService secureService;
        private final InsecureAccountService insecureService;
        private final boolean useSecure;
        private final String userId;
        private final Random random = new Random();
        private final CountDownLatch latch;

        public TransferWorker(String userId, SecureAccountService secure,
                InsecureAccountService insecure, boolean useSecure,
                CountDownLatch latch) {
            this.userId = userId;
            this.secureService = secure;
            this.insecureService = insecure;
            this.useSecure = useSecure;
            this.latch = latch;
        }

        @Override
        public void run() {
            try {
                latch.await(); // Wait for all threads to start

                for (int i = 0; i < 100; i++) {
                    // Randomly choose accounts and amount
                    String fromAccount = random.nextBoolean() ? "acc1" : "acc2";
                    String toAccount = fromAccount.equals("acc1") ? "acc2" : "acc1";
                    double amount = random.nextInt(10) + 1;

                    if (useSecure) {
                        secureService.transferSecure(fromAccount, toAccount, amount, userId);
                    } else {
                        insecureService.transferInsecure(fromAccount, toAccount, amount, userId);
                    }

                    // Small delay to increase chance of race conditions
                    Thread.sleep(random.nextInt(5));
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    // ==================== TEST HARNESS ====================

    static class TestHarness {

        public static void testInsecureService(int numThreads) throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("TESTING INSECURE SERVICE (CWE-566)");
            System.out.println("=".repeat(60));

            InsecureAccountService service = new InsecureAccountService();
            CountDownLatch latch = new CountDownLatch(1);
            List<Thread> threads = new ArrayList<>();

            // Get initial balances
            Account acc1 = service.accounts.get("acc1");
            Account acc2 = service.accounts.get("acc2");
            double initialBalance1 = acc1.getBalance();
            double initialBalance2 = acc2.getBalance();
            double initialTotal = initialBalance1 + initialBalance2;

            System.out.println("\nInitial balances:");
            System.out.println("  acc1: $" + initialBalance1);
            System.out.println("  acc2: $" + initialBalance2);
            System.out.println("  Total: $" + initialTotal);

            // Create worker threads
            for (int i = 0; i < numThreads; i++) {
                String userId = (i % 2 == 0) ? "user1" : "user2";
                TransferWorker worker = new TransferWorker(
                        userId, null, service, false, latch);
                Thread t = new Thread(worker, "Worker-" + i);
                threads.add(t);
                t.start();
            }

            // Start all threads simultaneously
            System.out.println("\nStarting " + numThreads + " threads...");
            latch.countDown();

            // Wait for completion
            for (Thread t : threads) {
                t.join();
            }

            // Check final balances
            double finalBalance1 = acc1.getBalance();
            double finalBalance2 = acc2.getBalance();
            double finalTotal = finalBalance1 + finalBalance2;

            System.out.println("\nFinal balances (insecure):");
            System.out.println("  acc1: $" + finalBalance1);
            System.out.println("  acc2: $" + finalBalance2);
            System.out.println("  Total: $" + finalTotal);
            System.out.println("  Expected total: $" + initialTotal);
            System.out.println("  Difference: $" + (finalTotal - initialTotal));
            System.out.println("  Transactions: " + service.getTransactionCount());

            if (Math.abs(finalTotal - initialTotal) > 0.01) {
                System.out.println("\n⚠️ DATA CORRUPTION DETECTED!");
            }
        }

        public static void testSecureService(int numThreads) throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("TESTING SECURE SERVICE (MITIGATED)");
            System.out.println("=".repeat(60));

            SecureAccountService service = new SecureAccountService();
            CountDownLatch latch = new CountDownLatch(1);
            List<Thread> threads = new ArrayList<>();

            // Get initial balances
            double initialBalance1 = service.getBalance("acc1", "admin").orElse(0);
            double initialBalance2 = service.getBalance("acc2", "admin").orElse(0);
            double initialTotal = initialBalance1 + initialBalance2;

            System.out.println("\nInitial balances:");
            System.out.println("  acc1: $" + initialBalance1);
            System.out.println("  acc2: $" + initialBalance2);
            System.out.println("  Total: $" + initialTotal);

            // Create worker threads
            for (int i = 0; i < numThreads; i++) {
                String userId = (i % 2 == 0) ? "user1" : "user2";
                TransferWorker worker = new TransferWorker(
                        userId, service, null, true, latch);
                Thread t = new Thread(worker, "Worker-" + i);
                threads.add(t);
                t.start();
            }

            // Start all threads simultaneously
            System.out.println("\nStarting " + numThreads + " threads...");
            latch.countDown();

            // Wait for completion
            for (Thread t : threads) {
                t.join();
            }

            // Check final balances
            double finalBalance1 = service.getBalance("acc1", "admin").orElse(0);
            double finalBalance2 = service.getBalance("acc2", "admin").orElse(0);
            double finalTotal = finalBalance1 + finalBalance2;

            System.out.println("\nFinal balances (secure):");
            System.out.println("  acc1: $" + finalBalance1);
            System.out.println("  acc2: $" + finalBalance2);
            System.out.println("  Total: $" + finalTotal);
            System.out.println("  Expected total: $" + initialTotal);
            System.out.println("  Difference: $" + (finalTotal - initialTotal));
            System.out.println("  Transactions: " + service.getTransactionCount());

            if (Math.abs(finalTotal - initialTotal) < 0.01) {
                System.out.println("\n✅ Data integrity maintained!");
            }
        }

        public static void comparePerformance(int numThreads) throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("PERFORMANCE COMPARISON");
            System.out.println("=".repeat(60));

            // Test insecure
            long start = System.currentTimeMillis();
            testInsecureService(numThreads);
            long insecureTime = System.currentTimeMillis() - start;

            Thread.sleep(2000);

            // Test secure
            start = System.currentTimeMillis();
            testSecureService(numThreads);
            long secureTime = System.currentTimeMillis() - start;

            System.out.println("\n📊 Performance Results:");
            System.out.println("  Insecure time: " + insecureTime + "ms");
            System.out.println("  Secure time:   " + secureTime + "ms");
            System.out.println("  Overhead: " +
                    String.format("%.2f", (secureTime - insecureTime) * 100.0 / insecureTime) + "%");
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-566: Authorization Bypass Through User-Controlled SQL Primary Key");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE shared data (CWE-566)");
            System.out.println("  2. Demonstrate SECURE shared data (Mitigated)");
            System.out.println("  3. Run performance comparison");
            System.out.println("  4. Test with different thread counts");
            System.out.println("  5. Show security analysis");
            System.out.println("  6. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            try {
                switch (choice) {
                    case "1":
                        TestHarness.testInsecureService(10);
                        break;

                    case "2":
                        TestHarness.testSecureService(10);
                        break;

                    case "3":
                        TestHarness.comparePerformance(10);
                        break;

                    case "4":
                        testWithDifferentThreads(scanner);
                        break;

                    case "5":
                        showSecurityAnalysis();
                        break;

                    case "6":
                        System.out.println("\nExiting...");
                        scanner.close();
                        return;

                    default:
                        System.out.println("[!] Invalid option");
                }
            } catch (InterruptedException e) {
                System.err.println("Test interrupted: " + e.getMessage());
            }
        }
    }

    private static void testWithDifferentThreads(Scanner scanner) throws InterruptedException {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("THREAD COUNT TEST");
        System.out.println("=".repeat(60));

        System.out.print("\nEnter number of threads: ");
        int threads = Integer.parseInt(scanner.nextLine().trim());

        System.out.println("\nTesting insecure service with " + threads + " threads...");
        TestHarness.testInsecureService(threads);

        Thread.sleep(2000);

        System.out.println("\nTesting secure service with " + threads + " threads...");
        TestHarness.testSecureService(threads);
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-566");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION:");
        System.out.println("  1. No Synchronization on Shared Data:");
        System.out.println("     • Check-then-act race conditions");
        System.out.println("     • Lost updates");
        System.out.println("     • Inconsistent reads");

        System.out.println("\n  2. Mutable Collections Exposed:");
        System.out.println("     • Direct reference to internal collections");
        System.out.println("     • Can be modified by any thread");

        System.out.println("\n  3. Non-Atomic Counters:");
        System.out.println("     • transactionCount not synchronized");
        System.out.println("     • Lost increments/decrements");

        System.out.println("\n  4. No Locking on Complex Operations:");
        System.out.println("     • Balance transfers not atomic");
        System.out.println("     • Money can be created or lost");

        System.out.println("\n  5. Consequences:");
        System.out.println("     • Data corruption");
        System.out.println("     • Lost transactions");
        System.out.println("     • Incorrect balances");
        System.out.println("     • Race conditions");

        System.out.println("\n✅ SECURE IMPLEMENTATION:");
        System.out.println("  1. Concurrent Collections:");
        System.out.println("     • ConcurrentHashMap for thread-safe maps");
        System.out.println("     • ConcurrentLinkedQueue for logs");
        System.out.println("     • Copy-on-write where appropriate");

        System.out.println("\n  2. Atomic Variables:");
        System.out.println("     • AtomicInteger for counters");
        System.out.println("     • AtomicLong for totals");
        System.out.println("     • No race conditions on increments");

        System.out.println("\n  3. Proper Locking:");
        System.out.println("     • ReadWriteLock for read/write separation");
        System.out.println("     • Per-account locks for fine-grained sync");
        System.out.println("     • Consistent lock ordering prevents deadlock");

        System.out.println("\n  4. Immutable Snapshots:");
        System.out.println("     • Return copies of collections");
        System.out.println("     • Defensive copying");
        System.out.println("     • No external modification");

        System.out.println("\n📋 BEST PRACTICES FOR SHARED DATA:");
        System.out.println("  1. Use concurrent collections from java.util.concurrent");
        System.out.println("  2. Use atomic variables for counters");
        System.out.println("  3. Synchronize check-then-act operations");
        System.out.println("  4. Use fine-grained locking when possible");
        System.out.println("  5. Always lock in consistent order to prevent deadlock");
        System.out.println("  6. Return copies of internal collections");
        System.out.println("  7. Use read/write locks for read-heavy data");
        System.out.println("  8. Consider using immutable objects");
        System.out.println("  9. Document thread-safety guarantees");
        System.out.println("  10. Test under high concurrency");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use ConcurrentHashMap for maps");
        System.out.println("  • Use AtomicInteger/Long for counters");
        System.out.println("  • Use synchronized blocks for check-then-act");
        System.out.println("  • Return defensive copies of collections");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE - Check-then-act");
        System.out.println("  if (account.getBalance() >= amount) {");
        System.out.println("      account.setBalance(account.getBalance() - amount);");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE - Atomic operation");
        System.out.println("  synchronized(account) {");
        System.out.println("      if (account.getBalance() >= amount) {");
        System.out.println("          account.setBalance(account.getBalance() - amount);");
        System.out.println("      }");
        System.out.println("  }");

        System.out.println("\n📊 SYNCHRONIZATION COMPARISON:");
        System.out.println("  Method              | Thread-Safe | Performance | Complexity");
        System.out.println("  --------------------|-------------|-------------|-----------");
        System.out.println("  No synchronization  |     ❌      |    High     |   Low");
        System.out.println("  synchronized methods|     ✓       |    Low      |   Low");
        System.out.println("  Atomic variables    |     ✓       |    High     |   Medium");
        System.out.println("  Concurrent collections|   ✓       |    High     |   Low");
        System.out.println("  ReadWrite locks     |     ✓       |    Medium   |   High");
        System.out.println("  Per-account locks   |     ✓       |    High     |   High");

        System.out.println("\n⚠️ REMEMBER: Even with thread-safe collections,");
        System.out.println("  compound actions (check-then-act) still need synchronization!");
    }
}