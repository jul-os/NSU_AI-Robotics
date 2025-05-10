#define MAX 1000005
#include <stdio.h>
#include <stdlib.h>

int ways_arr[610][610];
int parents[610][610];
int pth[MAX];
void FW_alg(int n)
{
    for (int k = 1; k <= n; k++)
    {
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                if (ways_arr[i][j] > ways_arr[i][k] + ways_arr[k][j])
                {
                    ways_arr[i][j] = ways_arr[i][k] + ways_arr[k][j];
                    parents[i][j] = parents[i][k];
                }
            }
        }
    }
}

int pth_find(int start, int end)
{
    if (parents[start][end] == -1)
        return 0;
    int ind = 0;
    pth[ind++] = start;
    while (start != end)
    {
        start = parents[start][end];
        pth[ind++] = start;
    }
    return ind;
}

int main()
{
    FILE *input, *output;
    input = freopen("input.txt", "r", stdin);
    output = freopen("output.txt", "w", stdout);
    int n, m, p, k;
    scanf("%d %d %d %d", &n, &m, &p, &k);
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            ways_arr[i][i] = 0;
            ways_arr[i][j] = MAX;
            parents[i][j] = 0;
        }
    }
    int start, end, len;
    for (int i = 0; i < m; i++)
    {
        scanf("%d %d %d", &start, &end, &len);
        if (ways_arr[start][end] > len)
        {
            ways_arr[start][end] = len;
            ways_arr[end][start] = len;
        }
        parents[start][end] = end;
    }
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (ways_arr[i][j] == MAX)
            {
                parents[i][j] = -1;
            }
            else
                parents[i][j] = j;
        }
    }
    FW_alg(n);
    int x, y, idx;
    for (int i = 0; i < p; i++)
    {
        scanf("%d %d", &x, &y);
        idx = pth_find(x, y);
        printf("%d  ", ways_arr[x][y]);
        printf("%d  ", idx);
        for (int j = 0; j < idx; j++)
        {
            printf("%d ", pth[j]);
        }
        printf("\n");
    }
    for (int i = 0; i < k; i++)
    {
        scanf("%d %d", &x, &y);
        printf(" %d\n", ways_arr[x][y]);
    }
    return 0;
}