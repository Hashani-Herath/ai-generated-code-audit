#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

#define MAX_WORDS 1000
#define MAX_WORD_LEN 50
#define MAX_LOG_ENTRIES 1000

// Word frequency tracker
typedef struct {
    char word[MAX_WORD_LEN];
    int count;
    int total_occurrences;
} WordFrequency;

typedef struct {
    WordFrequency words[MAX_WORDS];
    int count;
    pthread_mutex_t mutex;
} WordStats;

// Log entry with metadata
typedef struct {
    char raw_message[1024];
    time_t timestamp;
    pthread_t thread_id;
    int priority;  // 1-5
    char processed_words[MAX_WORDS][MAX_WORD_LEN];
    int word_count;
} LogEntry;

// Global statistics
static WordStats word_stats = {
    .count = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

// Update word frequency
void update_word_frequency(const char *word) {
    pthread_mutex_lock(&word_stats.mutex);
    
    // Convert to lowercase for consistent counting
    char lower_word[MAX_WORD_LEN];
    for (int i = 0; word[i] && i < MAX_WORD_LEN-1; i++) {
        lower_word[i] = tolower(word[i]);
    }
    lower_word[MAX_WORD_LEN-1] = '\0';
    
    // Look for existing word
    int found = -1;
    for (int i = 0; i < word_stats.count; i++) {
        if (strcmp(word_stats.words[i].word, lower_word) == 0) {
            found = i;
            break;
        }
    }
    
    if (found >= 0) {
        word_stats.words[found].count++;
    } else if (word_stats.count < MAX_WORDS) {
        strcpy(word_stats.words[word_stats.count].word, lower_word);
        word_stats.words[word_stats.count].count = 1;
        word_stats.count++;
    }
    
    pthread_mutex_unlock(&word_stats.mutex);
}

// Process log entry with strtok
void process_log_entry(LogEntry *entry) {
    char *token;
    char *saveptr;
    char line_copy[1024];
    
    strncpy(line_copy, entry->raw_message, sizeof(line_copy) - 1);
    
    entry->word_count = 0;
    token = strtok_r(line_copy, " .,;:!?()[]{}\t\n\r", &saveptr);
    
    while (token != NULL && entry->word_count < MAX_WORDS) {
        strncpy(entry->processed_words[entry->word_count], token, MAX_WORD_LEN - 1);
        entry->processed_words[entry->word_count][MAX_WORD_LEN - 1] = '\0';
        
        // Update global statistics
        update_word_frequency(token);
        
        entry->word_count++;
        token = strtok_r(NULL, " .,;:!?()[]{}\t\n\r", &saveptr);
    }
}

// Thread-safe log queue
typedef struct LogNode {
    LogEntry entry;
    struct LogNode *next;
} LogNode;

typedef struct {
    LogNode *head;
    LogNode *tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
} LogQueue;

static struct {
    LogQueue queue;
    FILE *log_file;
    pthread_t processor_thread;
    int running;
    pthread_mutex_t file_mutex;
} logger = {
    .queue = {.head = NULL, .tail = NULL, .count = 0, 
              .mutex = PTHREAD_MUTEX_INITIALIZER, 
              .not_empty = PTHREAD_COND_INITIALIZER},
    .running = 1,
    .file_mutex = PTHREAD_MUTEX_INITIALIZER
};

void init_enhanced_logger(const char *filename) {
    logger.log_file = fopen(filename, "w");
    if (!logger.log_file) {
        perror("Failed to open log file");
        exit(1);
    }
    
    fprintf(logger.log_file, "=== Enhanced Logger Started at %ld ===\n", time(NULL));
    fprintf(logger.log_file, "Format: [timestamp] thread_id priority | words...\n");
    fflush(logger.log_file);
}

void log_event(int priority, const char *format, ...) {
    LogNode *node = (LogNode*)malloc(sizeof(LogNode));
    if (!node) return;
    
    va_list args;
    va_start(args, format);
    vsnprintf(node->entry.raw_message, sizeof(node->entry.raw_message), format, args);
    va_end(args);
    
    node->entry.timestamp = time(NULL);
    node->entry.thread_id = pthread_self();
    node->entry.priority = priority;
    node->next = NULL;
    
    pthread_mutex_lock(&logger.queue.mutex);
    
    if (logger.queue.tail) {
        logger.queue.tail->next = node;
        logger.queue.tail = node;
    } else {
        logger.queue.head = node;
        logger.queue.tail = node;
    }
    logger.queue.count++;
    
    pthread_cond_signal(&logger.queue.not_empty);
    pthread_mutex_unlock(&logger.queue.mutex);
}

void* log_processor_thread(void* arg) {
    char time_str[26];
    
    printf("Log processor thread started\n");
    
    while (logger.running) {
        pthread_mutex_lock(&logger.queue.mutex);
        
        while (logger.queue.count == 0 && logger.running) {
            pthread_cond_wait(&logger.queue.not_empty, &logger.queue.mutex);
        }
        
        if (!logger.running && logger.queue.count == 0) {
            pthread_mutex_unlock(&logger.queue.mutex);
            break;
        }
        
        LogNode *node = logger.queue.head;
        if (node) {
            logger.queue.head = node->next;
            if (!logger.queue.head) {
                logger.queue.tail = NULL;
            }
            logger.queue.count--;
            
            pthread_mutex_unlock(&logger.queue.mutex);
            
            // Process the log entry
            process_log_entry(&node->entry);
            
            // Write to file
            pthread_mutex_lock(&logger.file_mutex);
            
            ctime_r(&node->entry.timestamp, time_str);
            time_str[24] = '\0';
            
            fprintf(logger.log_file, "[%s] Thread %lu [P%d] | ", 
                    time_str, (unsigned long)node->entry.thread_id, 
                    node->entry.priority);
            
            for (int i = 0; i < node->entry.word_count; i++) {
                fprintf(logger.log_file, "%s ", node->entry.processed_words[i]);
            }
            fprintf(logger.log_file, "\n");
            fflush(logger.log_file);
            
            pthread_mutex_unlock(&logger.file_mutex);
            
            free(node);
        } else {
            pthread_mutex_unlock(&logger.queue.mutex);
        }
    }
    
    return NULL;
}

void print_word_statistics(void) {
    printf("\n=== Word Frequency Statistics ===\n");
    printf("%-20s %10s\n", "Word", "Count");
    printf("------------------------------\n");
    
    pthread_mutex_lock(&word_stats.mutex);
    
    // Sort by count (simple bubble sort for demo)
    for (int i = 0; i < word_stats.count - 1; i++) {
        for (int j = 0; j < word_stats.count - i - 1; j++) {
            if (word_stats.words[j].count < word_stats.words[j + 1].count) {
                WordFrequency temp = word_stats.words[j];
                word_stats.words[j] = word_stats.words[j + 1];
                word_stats.words[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < word_stats.count && i < 20; i++) {
        printf("%-20s %10d\n", 
               word_stats.words[i].word, 
               word_stats.words[i].count);
    }
    
    pthread_mutex_unlock(&word_stats.mutex);
}

void* worker_thread_enhanced(void* arg) {
    int thread_id = *(int*)arg;
    const char *actions[] = {"READ", "WRITE", "DELETE", "UPDATE", "CREATE"};
    const char *statuses[] = {"SUCCESS", "FAILED", "PENDING", "RETRY"};
    
    for (int i = 0; i < 5; i++) {
        int priority = rand() % 3 + 1;  // 1-3
        int action_idx = rand() % 5;
        int status_idx = rand() % 4;
        
        log_event(priority, "Thread %d: %s operation %s for item %d", 
                  thread_id, actions[action_idx], statuses[status_idx], i);
        
        usleep(rand() % 300000);
    }
    
    return NULL;
}

int main() {
    pthread_t threads[5];
    int thread_ids[5];
    
    printf("Enhanced Multithreaded Logger with Word Analysis\n");
    printf("================================================\n\n");
    
    srand(time(NULL));
    
    // Initialize logger
    init_enhanced_logger("enhanced_log.txt");
    
    // Start processor thread
    pthread_create(&logger.processor_thread, NULL, log_processor_thread, NULL);
    
    // Create worker threads
    for (int i = 0; i < 5; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, worker_thread_enhanced, &thread_ids[i]);
    }
    
    // Main thread logs
    log_event(5, "SYSTEM: Logger initialized with 5 worker threads");
    log_event(4, "CONFIG: max_log_size=10MB retention_days=30");
    
    // Wait for workers
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Wait for processing
    sleep(1);
    
    // Shutdown
    logger.running = 0;
    pthread_cond_signal(&logger.queue.not_empty);
    pthread_join(logger.processor_thread, NULL);
    
    if (logger.log_file) {
        fclose(logger.log_file);
    }
    
    // Print statistics
    print_word_statistics();
    
    printf("\nAll done! Check enhanced_log.txt for detailed logs.\n");
    
    return 0;
}