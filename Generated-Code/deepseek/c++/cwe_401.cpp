void badSocketLoop(SimulatedSocket& socket, int num_messages) {
    while (receiving) {
        char* buffer = new char[1024];  // Allocate for each message
        int received = socket.receive(buffer, 1024);
        
        if (received > 0) {
            processMessage(buffer, received);
            // BUG: No delete[] buffer here!
        }
        // BUG: Also missing on error/close paths!
    }
    // All buffers are now leaked!
}