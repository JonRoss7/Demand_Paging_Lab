import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

def create_timeline_graph(log_file='events.log', output_file='timeline.png'):
    """
    Reads the simulation log and generates a timeline graph.
    """
    try:
        # Read all data as strings first to prevent parsing errors
        df = pd.read_csv(log_file, dtype=str)
        
        # Convert 'Time' column to numeric. 
        # errors='coerce' will turn any bad values into 'NaN'
        df['Time'] = pd.to_numeric(df['Time'], errors='coerce')
        
        # Drop any rows that failed to parse (e.g., the header or malformed lines)
        df = df.dropna(subset=['Time'])
        
    except FileNotFoundError:
        print(f"Error: Log file '{log_file}' not found.")
        print("Please run the simulator first to generate the log file.")
        return
    except pd.errors.EmptyDataError:
        print(f"Error: Log file '{log_file}' is empty.")
        return
    except Exception as e:
        print(f"An error occurred reading the log file: {e}")
        return

    # Clean up any "nan" strings that might have come through
    df = df.fillna('')

    # Create a unique identifier for each thread
    df['PID'] = df['PID'].str.replace(r'\.0$', '', regex=True)
    df['ThreadID'] = df['ThreadID'].str.replace(r'\.0$', '', regex=True)
    
    df['ThreadName'] = 'PID ' + df['PID'] + ', Thread ' + df['ThreadID']
    
    # Filter out any bad rows
    df = df[df['PID'] != '']
    
    # Get unique thread names in order of first appearance
    thread_names = df['ThreadName'].unique()
    
    # Create a y-axis mapping for threads
    thread_map = {name: i for i, name in enumerate(thread_names)}
    
    # --- FIX ---
    # Strip whitespace from 'Event' column to fix matching.
    df['Event'] = df['Event'].str.strip()
    
    # Get event data
    cpu_events = df[df['Event'] == 'CPU_EXECUTE']
    tlb_misses = df[df['Event'] == 'TLB_MISS']
    page_faults = df[df['Event'] == 'PAGE_FAULT']
    
    # --- DEBUGGING ---
    print(f"Found {len(cpu_events)} CPU execute events.")
    print(f"Found {len(tlb_misses)} TLB miss events.")
    print(f"Found {len(page_faults)} Page fault events.")
    # --- END DEBUGGING ---
    
    fig, ax = plt.subplots(figsize=(20, 10))
    
    # --- 1. Plot CPU Bursts (as horizontal bars) ---
    for thread_name, y_pos in thread_map.items():
        thread_cpu_events = cpu_events[cpu_events['ThreadName'] == thread_name]
        
        ax.barh(y_pos, width=1, left=thread_cpu_events['Time'], 
                align='center', height=0.5, color='blue', alpha=0.6)

    # --- 2. Plot Memory Events (as markers) ---
    
    if not tlb_misses.empty:
        y_vals_tlb = tlb_misses['ThreadName'].map(thread_map)
        ax.plot(tlb_misses['Time'] + 0.5, y_vals_tlb, 'o', 
                color='orange', markersize=8, label='TLB Miss')
    
    if not page_faults.empty:
        y_vals_pf = page_faults['ThreadName'].map(thread_map)
        ax.plot(page_faults['Time'] + 0.5, y_vals_pf, 'X', 
                color='red', markersize=10, label='Page Fault')

    # --- 3. Format the Graph ---
    ax.set_yticks(range(len(thread_names)))
    ax.set_yticklabels(thread_names)
    ax.set_xlabel('Simulation Time')
    ax.set_ylabel('Threads')
    ax.set_title('Simulation Timeline')
    ax.grid(axis='x', linestyle=':', alpha=0.7)
    
    ax.invert_yaxis()
    
    handles, labels = ax.get_legend_handles_labels()
    
    patches = [
        mpatches.Patch(color='blue', alpha=0.6, label='CPU Burst'),
    ]
    handles.extend(patches)
    
    if handles:
        ax.legend(handles=handles, loc='upper right')

    
    plt.savefig(output_file)
    print(f"Successfully generated graph: {output_file}")

if __name__ == "__main__":
    create_timeline_graph()