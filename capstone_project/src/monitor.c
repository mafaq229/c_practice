/*
 * monitor.c - System Monitor (Part E)
 *
 * A monitoring tool that collects and displays statistics from
 * the various components of the Mini-GIOS system.
 *
 * Features:
 * - Real-time statistics display
 * - Cache hit/miss rates
 * - Thread pool utilization
 * - Request throughput
 *
 * This demonstrates monitoring and observability concepts.
 *
 * Usage: ./monitor [options]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include "../include/protocol.h"
#include "../include/cache.h"

/* ============================================================================
 * Configuration
 * ============================================================================ */

#define REFRESH_INTERVAL_MS     1000    /* Refresh every second */
#define HISTORY_SIZE            60      /* Keep 60 samples for graphing */

static volatile sig_atomic_t running = 1;

/* ============================================================================
 * Statistics Structures
 * ============================================================================ */

typedef struct {
    /* Request statistics */
    unsigned long total_requests;
    unsigned long successful_requests;
    unsigned long failed_requests;
    double requests_per_second;

    /* Cache statistics */
    unsigned long cache_hits;
    unsigned long cache_misses;
    double cache_hit_rate;
    size_t cache_size;
    size_t cache_max_size;
    int cache_entries;
    unsigned long cache_evictions;

    /* Thread pool statistics */
    int active_workers;
    int total_workers;
    int queue_size;
    int tasks_completed;

    /* Timing */
    double avg_response_time_ms;
    double max_response_time_ms;

    /* History for trends */
    double rps_history[HISTORY_SIZE];
    int history_index;

    /* Timestamp */
    time_t timestamp;

} system_stats_t;

static system_stats_t stats;

/* ============================================================================
 * Signal Handler
 * ============================================================================ */

static void signal_handler(int sig) {
    (void)sig;
    running = 0;
}

/* ============================================================================
 * Statistics Collection
 * ============================================================================ */

/*
 * collect_stats - Gather statistics from various sources
 */
static void collect_stats(void) {
    /*
     * TODO: Implement statistics collection
     *
     * In a real system, you would:
     * 1. Read from shared memory (if stats are stored there)
     * 2. Query via IPC (message queues)
     * 3. Read from log files
     * 4. Connect to components via a stats API
     *
     * For this exercise, we'll simulate some statistics.
     * When you integrate with the actual components,
     * replace this with real data collection.
     */

    stats.timestamp = time(NULL);

    /* Simulate request statistics */
    static unsigned long prev_requests = 0;
    static time_t prev_time = 0;

    if (prev_time > 0) {
        time_t elapsed = stats.timestamp - prev_time;
        if (elapsed > 0) {
            unsigned long new_requests = stats.total_requests - prev_requests;
            stats.requests_per_second = (double)new_requests / elapsed;
        }
    }
    prev_requests = stats.total_requests;
    prev_time = stats.timestamp;

    /* Calculate cache hit rate */
    unsigned long total_cache_ops = stats.cache_hits + stats.cache_misses;
    if (total_cache_ops > 0) {
        stats.cache_hit_rate = (double)stats.cache_hits / total_cache_ops * 100.0;
    } else {
        stats.cache_hit_rate = 0.0;
    }

    /* Update history */
    stats.rps_history[stats.history_index] = stats.requests_per_second;
    stats.history_index = (stats.history_index + 1) % HISTORY_SIZE;
}

/* ============================================================================
 * Display Functions
 * ============================================================================ */

/*
 * clear_screen - Clear terminal screen
 */
static void clear_screen(void) {
    printf("\033[2J\033[H");  /* ANSI escape codes */
}

/*
 * draw_bar - Draw a progress bar
 */
static void draw_bar(double value, double max, int width) {
    int filled = (max > 0) ? (int)(value / max * width) : 0;
    if (filled > width) filled = width;

    printf("[");
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("]");
}

/*
 * draw_sparkline - Draw a simple sparkline graph
 */
static void draw_sparkline(double *values, int count, int width) {
    static const char *blocks[] = {" ", "▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};

    /* Find min/max */
    double min_val = values[0], max_val = values[0];
    for (int i = 1; i < count; i++) {
        if (values[i] < min_val) min_val = values[i];
        if (values[i] > max_val) max_val = values[i];
    }
    double range = max_val - min_val;
    if (range < 0.001) range = 1.0;

    /* Draw sparkline */
    int start = (count > width) ? count - width : 0;
    for (int i = start; i < count; i++) {
        int level = (int)((values[i] - min_val) / range * 7);
        if (level < 0) level = 0;
        if (level > 7) level = 7;
        printf("%s", blocks[level]);
    }
}

/*
 * display_stats - Render the statistics display
 */
static void display_stats(void) {
    /*
     * TODO: Implement a nice statistics display
     *
     * Display:
     * - Request throughput
     * - Cache statistics
     * - Thread pool utilization
     * - Graphs/trends
     */

    clear_screen();

    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║          MINI-GIOS SYSTEM MONITOR                              ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");

    /* Timestamp */
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&stats.timestamp));
    printf("║ Time: %-57s ║\n", time_buf);
    printf("╠════════════════════════════════════════════════════════════════╣\n");

    /* Request Statistics */
    printf("║ REQUESTS                                                        ║\n");
    printf("║   Total:     %-10lu   Successful: %-10lu                ║\n",
           stats.total_requests, stats.successful_requests);
    printf("║   Failed:    %-10lu   RPS: %-8.1f                       ║\n",
           stats.failed_requests, stats.requests_per_second);
    printf("║   RPS Trend: ");
    draw_sparkline(stats.rps_history, HISTORY_SIZE, 40);
    printf("           ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");

    /* Cache Statistics */
    printf("║ CACHE                                                           ║\n");
    printf("║   Entries: %-5d   Size: %zuKB / %zuKB                          ║\n",
           stats.cache_entries,
           stats.cache_size / 1024,
           stats.cache_max_size / 1024);
    printf("║   Usage: ");
    draw_bar((double)stats.cache_size, (double)stats.cache_max_size, 40);
    printf(" %3.0f%% ║\n", (double)stats.cache_size / stats.cache_max_size * 100);
    printf("║   Hits: %-10lu   Misses: %-10lu   Rate: %5.1f%%       ║\n",
           stats.cache_hits, stats.cache_misses, stats.cache_hit_rate);
    printf("║   Evictions: %-10lu                                         ║\n",
           stats.cache_evictions);
    printf("╠════════════════════════════════════════════════════════════════╣\n");

    /* Thread Pool Statistics */
    printf("║ THREAD POOL                                                     ║\n");
    printf("║   Workers: %d/%d active   Queue: %d pending                    ║\n",
           stats.active_workers, stats.total_workers, stats.queue_size);
    printf("║   Load: ");
    draw_bar((double)stats.active_workers, (double)stats.total_workers, 40);
    printf(" %3.0f%% ║\n",
           (double)stats.active_workers / stats.total_workers * 100);
    printf("║   Tasks Completed: %-10d                                   ║\n",
           stats.tasks_completed);
    printf("╠════════════════════════════════════════════════════════════════╣\n");

    /* Timing */
    printf("║ RESPONSE TIME                                                   ║\n");
    printf("║   Average: %-8.2f ms   Max: %-8.2f ms                     ║\n",
           stats.avg_response_time_ms, stats.max_response_time_ms);
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\nPress Ctrl+C to exit\n");

    fflush(stdout);
}

/* ============================================================================
 * Main Loop
 * ============================================================================ */

static int run_monitor(void) {
    /*
     * TODO: Implement the monitor main loop
     *
     * Steps:
     * 1. Initialize statistics
     * 2. Main loop:
     *    - Collect statistics
     *    - Display statistics
     *    - Sleep for refresh interval
     * 3. Clean up
     */

    /* Initialize with some default values for demo */
    memset(&stats, 0, sizeof(stats));
    stats.cache_max_size = 10 * 1024 * 1024;  /* 10 MB */
    stats.total_workers = 4;

    /* Simulate some initial data */
    stats.total_requests = 1000;
    stats.successful_requests = 980;
    stats.failed_requests = 20;
    stats.cache_hits = 700;
    stats.cache_misses = 300;
    stats.cache_entries = 50;
    stats.cache_size = 2 * 1024 * 1024;
    stats.tasks_completed = 980;
    stats.active_workers = 2;
    stats.queue_size = 5;
    stats.avg_response_time_ms = 12.5;
    stats.max_response_time_ms = 150.0;

    printf("Starting monitor...\n");
    sleep(1);

    while (running) {
        collect_stats();
        display_stats();

        /* Simulate some activity for demo */
        stats.total_requests += (rand() % 50);
        stats.successful_requests += (rand() % 48);
        stats.cache_hits += (rand() % 30);
        stats.cache_misses += (rand() % 10);
        if (rand() % 10 == 0) {
            stats.cache_evictions++;
        }
        stats.tasks_completed = stats.successful_requests;
        stats.active_workers = 1 + (rand() % 4);
        stats.queue_size = rand() % 20;

        /* Sleep for refresh interval */
        usleep(REFRESH_INTERVAL_MS * 1000);
    }

    return 0;
}

/* ============================================================================
 * Main
 * ============================================================================ */

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("=== Mini-GIOS System Monitor ===\n");
    printf("This is a demo monitor with simulated data.\n");
    printf("In a real system, it would connect to running components.\n\n");

    int result = run_monitor();

    clear_screen();
    printf("Monitor stopped.\n");
    return result;
}
