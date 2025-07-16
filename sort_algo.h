/**
  ******************************************************************************
  * @file        : sort_algo.h
  * @author      : 
  * @version     : V1.0
  * @data        : 
  * @brief       : Generic sorting algorithms for various data structures
  * @attattention: None
  ******************************************************************************
  */

#ifndef __SORT_ALGO_H
#define __SORT_ALGO_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "sys_def.h"
#include "my_list.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Generic compare function typedef
 * @param a: First element to compare
 * @param b: Second element to compare
 * @return: < 0 if a < b, 0 if a == b, > 0 if a > b
 */
typedef int (*sort_compare_func_t)(const void *a, const void *b);

/**
 * @brief Generic swap function typedef
 * @param a: First element to swap
 * @param b: Second element to swap
 */
typedef void (*sort_swap_func_t)(void *a, void *b);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Insert a node into a sorted list
 * @param  head: Pointer to list head
 * @param  new_node: Node to insert
 * @param  compare_func: Function to compare nodes
 * @retval None
 */
void sorted_insert_list(list_t *head, list_t *new_node, sort_compare_func_t compare_func);

/**
 * @brief  Sort an array using insertion sort algorithm
 * @param  base: Pointer to the array to be sorted
 * @param  nmemb: Number of elements in the array
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 */
void insertion_sort(void *base, size_t nmemb, size_t size, sort_compare_func_t compare_func);

/**
 * @brief  Sort an array using quick sort algorithm
 * @param  base: Pointer to the array to be sorted
 * @param  nmemb: Number of elements in the array
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 */
void quick_sort(void *base, size_t nmemb, size_t size, sort_compare_func_t compare_func);

/**
 * @brief  Sort an array using merge sort algorithm
 * @param  base: Pointer to the array to be sorted
 * @param  nmemb: Number of elements in the array
 * @param  size: Size of each element
 * @param  compare_func: Function to compare elements
 * @retval None
 */
void merge_sort(void *base, size_t nmemb, size_t size, sort_compare_func_t compare_func);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SORT_ALGO_H */
