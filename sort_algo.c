/**
  ******************************************************************************
  * @file        : sort_algo.c
  * @author      : 
  * @version     : V1.0
  * @date        : 
  * @brief       : Generic sorting algorithms for various data structures
  * @attention   : None
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sort_algo.h"
#include <string.h>
#include <stdlib.h>  /* For malloc and free */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define INSERTION_SORT_THRESHOLD 16  /* Use insertion sort for small arrays */
#define STACK_BUFFER_SIZE 128       /* Stack buffer size for small swaps */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t temp_buffer[STACK_BUFFER_SIZE];  /* Static buffer for small swaps */

/* Private function prototypes -----------------------------------------------*/
static void swap_bytes(void *a, void *b, size_t size);
static void quick_sort_internal(void *base, size_t lo, size_t hi, size_t size, sort_compare_func_t compare_func);
static void merge(void *base, void *temp, size_t lo, size_t mid, size_t hi, size_t size, sort_compare_func_t compare_func);
static void merge_sort_internal(void *base, void *temp, size_t lo, size_t hi, size_t size, sort_compare_func_t compare_func);

/**
 * @brief  Insert a node into a sorted list (specific for linked list)
 * @param  head: Pointer to list head
 * @param  new_node: Node to insert
 * @param  compare_func: Function to compare nodes
 * @retval None
 * 
 * This function uses insertion sort algorithm which is efficient for small lists
 * and when elements are inserted one by one. Time complexity: O(n)
 */
void sorted_insert_list(list_t *head, list_t *new_node, sort_compare_func_t compare_func)
{
    list_t *current;
    
    /* If list is empty or node should be inserted before the head */
    if (list_empty(head) || compare_func((const void *)new_node, (const void *)head->next) < 0) {
        list_add(new_node, head);
        return;
    }
    
    /* Find the right position to insert */
    list_for_each(current, head) {
        /* Skip the head node */
        if (current == head) {  /* Replace list_is_head with direct comparison */
            continue;
        }
        
        /* If we've reached the end of the list */
        if (current->next == head) {
            list_add(new_node, current);
            return;
        }
        
        /* If new_node should be inserted before the next node */
        if (compare_func((const void *)new_node, (const void *)current->next) < 0) {
            list_add(new_node, current);
            return;
        }
    }
    
    /* If we reach here, add to the end of the list */
    list_add_tail(new_node, head);
}

/**
 * @brief  Swap bytes between two memory locations
 * @param  a: First memory location
 * @param  b: Second memory location
 * @param  size: Size of each element in bytes
 * @retval None
 */
static void swap_bytes(void *a, void *b, size_t size)
{
    uint8_t *pa = (uint8_t *)a;
    uint8_t *pb = (uint8_t *)b;
    
    if (a == b) return;
    
    if (size <= STACK_BUFFER_SIZE) {
        /* Use static buffer for small swaps */
        memcpy(temp_buffer, pa, size);
        memcpy(pa, pb, size);
        memcpy(pb, temp_buffer, size);
    } else {
        /* For larger elements, use word-sized swaps when possible */
        size_t i = 0;
        
        /* Align to word boundary if possible */
        while (i < size && ((uintptr_t)(pa + i) & (sizeof(size_t) - 1))) {
            uint8_t t = pa[i];
            pa[i] = pb[i];
            pb[i] = t;
            i++;
        }
        
        /* Use word-sized swaps for bulk of data */
        while (i + sizeof(size_t) <= size) {
            size_t t = *(size_t *)(pa + i);
            *(size_t *)(pa + i) = *(size_t *)(pb + i);
            *(size_t *)(pb + i) = t;
            i += sizeof(size_t);
        }
        
        /* Handle remaining bytes */
        while (i < size) {
            uint8_t t = pa[i];
            pa[i] = pb[i];
            pb[i] = t;
            i++;
        }
    }
}

/**
 * @brief  Sort an array using insertion sort algorithm
 * @param  base: Pointer to the array to be sorted
 * @param  nmemb: Number of elements in the array
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 * 
 * Insertion sort is efficient for small arrays or nearly sorted arrays.
 * Time complexity: O(n²) in worst case, O(n) in best case
 */
void insertion_sort(void *base, size_t nmemb, size_t size, sort_compare_func_t compare_func)
{
    uint8_t *p = (uint8_t *)base;
    size_t i, j;
    
    /* Check for empty or single-element array */
    if (nmemb <= 1) return;
    
    for (i = 1; i < nmemb; i++) {
        for (j = i; j > 0; j--) {
            uint8_t *current = p + (j * size);
            uint8_t *prev = p + ((j - 1) * size);
            
            if (compare_func(prev, current) > 0) {
                swap_bytes(prev, current, size);
            } else {
                /* Already in order, stop comparing */
                break;
            }
        }
    }
}

/**
 * @brief  Sort an array using quick sort algorithm
 * @param  base: Pointer to the array to be sorted
 * @param  nmemb: Number of elements in the array
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 * 
 * Quick sort is efficient for large arrays.
 * Time complexity: O(n log n) on average, O(n²) in worst case
 */
void quick_sort(void *base, size_t nmemb, size_t size, sort_compare_func_t compare_func)
{
    /* Check for empty or single-element array */
    if (nmemb <= 1) return;
    
    quick_sort_internal(base, 0, nmemb - 1, size, compare_func);
}

/**
 * @brief  Internal quick sort implementation with optimizations
 * @param  base: Pointer to the array to be sorted
 * @param  lo: Lower index
 * @param  hi: Higher index
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 */
static void quick_sort_internal(void *base, size_t lo, size_t hi, size_t size, sort_compare_func_t compare_func)
{
    while (lo < hi) {
        /* Use insertion sort for small arrays */
        if (hi - lo < INSERTION_SORT_THRESHOLD) {
            insertion_sort((uint8_t *)base + lo * size, hi - lo + 1, size, compare_func);
            return;
        }
        
        uint8_t *p = (uint8_t *)base;
        
        /* Choose median-of-three as pivot */
        size_t mid = lo + (hi - lo) / 2;
        uint8_t *pl = p + lo * size;
        uint8_t *pm = p + mid * size;
        uint8_t *ph = p + hi * size;
        
        if (compare_func(pl, pm) > 0) swap_bytes(pl, pm, size);
        if (compare_func(pm, ph) > 0) swap_bytes(pm, ph, size);
        if (compare_func(pl, pm) > 0) swap_bytes(pl, pm, size);
        
        /* Move pivot to end */
        swap_bytes(pm, ph, size);
        uint8_t *pivot = ph;
        
        /* Partition */
        size_t i = lo;
        size_t j;
        
        for (j = lo; j < hi; j++) {
            uint8_t *element = p + (j * size);
            if (compare_func(element, pivot) <= 0) {
                uint8_t *current = p + (i * size);
                swap_bytes(current, element, size);
                i++;
            }
        }
        
        uint8_t *current = p + (i * size);
        swap_bytes(current, pivot, size);
        
        /* Recursively sort smaller partition, iterate on larger partition */
        if (i - lo < hi - i) {
            quick_sort_internal(base, lo, i - 1, size, compare_func);
            lo = i + 1;  /* Tail recursion elimination */
        } else {
            quick_sort_internal(base, i + 1, hi, size, compare_func);
            hi = i - 1;  /* Tail recursion elimination */
        }
    }
}

/**
 * @brief  Sort an array using merge sort algorithm
 * @param  base: Pointer to the array to be sorted
 * @param  nmemb: Number of elements in the array
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 * 
 * Merge sort is stable and guarantees O(n log n) time complexity.
 */
void merge_sort(void *base, size_t nmemb, size_t size, sort_compare_func_t compare_func)
{
    /* Check for empty or single-element array */
    if (nmemb <= 1) return;
    
    /* Allocate temporary buffer */
    void *temp = malloc(nmemb * size);
    if (!temp) {
        /* Fall back to insertion sort if memory allocation fails */
        insertion_sort(base, nmemb, size, compare_func);
        return;
    }
    
    merge_sort_internal(base, temp, 0, nmemb - 1, size, compare_func);
    
    /* Free temporary buffer */
    free(temp);
}

/**
 * @brief  Internal merge sort implementation
 * @param  base: Pointer to the array to be sorted
 * @param  temp: Temporary buffer
 * @param  lo: Lower index
 * @param  hi: Higher index
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 */
static void merge_sort_internal(void *base, void *temp, size_t lo, size_t hi, size_t size, sort_compare_func_t compare_func)
{
    if (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        
        /* Recursively sort both halves */
        merge_sort_internal(base, temp, lo, mid, size, compare_func);
        merge_sort_internal(base, temp, mid + 1, hi, size, compare_func);
        
        /* Merge the sorted halves */
        merge(base, temp, lo, mid, hi, size, compare_func);
    }
}

/**
 * @brief  Merge two sorted subarrays
 * @param  base: Pointer to the array
 * @param  temp: Temporary buffer
 * @param  lo: Start index of first subarray
 * @param  mid: End index of first subarray
 * @param  hi: End index of second subarray
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 */
static void merge(void *base, void *temp, size_t lo, size_t mid, size_t hi, size_t size, sort_compare_func_t compare_func)
{
    uint8_t *p = (uint8_t *)base;
    uint8_t *t = (uint8_t *)temp;
    size_t i, j, k;
    
    /* Copy both parts into the temporary array */
    memcpy(t + (lo * size), p + (lo * size), (hi - lo + 1) * size);
    
    /* Set pointers for merging */
    i = lo;         /* First subarray index */
    j = mid + 1;    /* Second subarray index */
    k = lo;         /* Merged array index */
    
    /* Merge the two parts back into the original array */
    while (i <= mid && j <= hi) {
        if (compare_func(t + (i * size), t + (j * size)) <= 0) {
            memcpy(p + (k * size), t + (i * size), size);
            i++;
        } else {
            memcpy(p + (k * size), t + (j * size), size);
            j++;
        }
        k++;
    }
    
    /* Copy remaining elements from first subarray, if any */
    while (i <= mid) {
        memcpy(p + (k * size), t + (i * size), size);
        i++;
        k++;
    }
    
    /* No need to copy remaining elements from second subarray
       as they are already in their correct position */
}
