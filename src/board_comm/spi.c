#include "includes.h"
#include "gyro.h"
#include "spi.h"
#include "board_comm.h"

volatile spi_callback_function_pointer spiCallbackFunctionArray[3] = {0,};
volatile spi_irq_callback_function_pointer spiIrqCallbackFunctionArray[3] = {0,};


void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == BOARD_COMM_SPI)
    {
		HAL_GPIO_WritePin(BOARD_COMM_DATA_RDY_PORT, BOARD_COMM_DATA_RDY_PIN, 0);
    }
    else if(hspi->Instance == GYRO_SPI)
    {
		
    }
}

// this function is called the the SPI transfer is complete. The registered callback function will then be called
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    /* if(hspi->Instance == SPI1)
    {
		spiCallbackFunctionArray[0](hspi);
    }
    else  */
    if(hspi->Instance == BOARD_COMM_SPI)
    {
		spiCallbackFunctionArray[1](hspi);
    }
    else if(hspi->Instance == GYRO_SPI)
    {
		spiCallbackFunctionArray[2](hspi);
    }
}

static void init_handle(SPI_HandleTypeDef* spiHandle, IRQn_Type irq)
{
    HAL_SPI_DeInit(spiHandle);
    HAL_NVIC_DisableIRQ(irq);

    if (HAL_SPI_Init(spiHandle) != HAL_OK)
    {
        error_handler(SPI_INIT_FAILIURE);
    }
}

void spi_init(SPI_HandleTypeDef* spiHandle, SPI_TypeDef* instance, uint32_t baudscaler, uint32_t spi_mode, uint32_t irqn, uint32_t irqp, uint32_t irqsp)
{
    /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
    spiHandle->Instance               = instance;
    spiHandle->Init.Mode              = spi_mode;
    spiHandle->Init.BaudRatePrescaler = baudscaler;
    spiHandle->Init.Direction         = SPI_DIRECTION_2LINES;
    if(instance == BOARD_COMM_SPI)
    {
    spiHandle->Init.CLKPhase          = SPI_PHASE_1EDGE;
    spiHandle->Init.CLKPolarity       = SPI_POLARITY_LOW;
    }
    else
    {
    spiHandle->Init.CLKPhase          = SPI_PHASE_2EDGE;
    spiHandle->Init.CLKPolarity       = SPI_POLARITY_HIGH;
    }
    spiHandle->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    spiHandle->Init.CRCPolynomial     = 7;
    spiHandle->Init.DataSize          = SPI_DATASIZE_8BIT;
    spiHandle->Init.FirstBit          = SPI_FIRSTBIT_MSB;
    spiHandle->Init.NSS               = SPI_NSS_SOFT;
    spiHandle->Init.TIMode            = SPI_TIMODE_DISABLE;
    init_handle((spiHandle), irqn);

    HAL_NVIC_SetPriority(irqn, irqp, irqsp);
    HAL_NVIC_EnableIRQ(irqn);
}

void spi_dma_init(SPI_HandleTypeDef* spiHandle, DMA_HandleTypeDef* hdma_spi_rx, DMA_HandleTypeDef* hdma_spi_tx, DMA_Channel_TypeDef* rxDmaChannel, DMA_Channel_TypeDef* txDmaChannel, uint32_t rxDmaIrqn, uint32_t txDmaIrqn)
{

    (*hdma_spi_rx).Instance = rxDmaChannel;
    (*hdma_spi_rx).Init.Direction = DMA_PERIPH_TO_MEMORY;
    (*hdma_spi_rx).Init.PeriphInc = DMA_PINC_DISABLE;
    (*hdma_spi_rx).Init.MemInc = DMA_MINC_ENABLE;
    (*hdma_spi_rx).Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    (*hdma_spi_rx).Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    if(spiHandle->Instance == BOARD_COMM_SPI)
    {
        (*hdma_spi_rx).Init.Mode = DMA_NORMAL;
    }
    else
    {
        (*hdma_spi_rx).Init.Mode = DMA_NORMAL;
    }
    
    (*hdma_spi_rx).Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(hdma_spi_rx) != HAL_OK)
    {
        error_handler(SPI_RX_DMA_INIT_FAILIURE);
    }

    __HAL_LINKDMA(spiHandle,hdmarx,*hdma_spi_rx);

    if(spiHandle->Instance == BOARD_COMM_SPI)
    {
        HAL_NVIC_SetPriority(rxDmaIrqn, BOARD_COMM_SPI_DMA_RX_PRE_PRI, BOARD_COMM_SPI_DMA_RX_SUB_PRI);
        HAL_NVIC_EnableIRQ(rxDmaIrqn);
    }
    else
    {
        HAL_NVIC_SetPriority(rxDmaIrqn, GYRO_SPI_DMA_RX_PRE_PRI, GYRO_SPI_DMA_RX_SUB_PRI);
        HAL_NVIC_EnableIRQ(rxDmaIrqn);
    }

    /* SPI3_TX Init */
    (*hdma_spi_tx).Instance = txDmaChannel;
    (*hdma_spi_tx).Init.Direction = DMA_MEMORY_TO_PERIPH;
    (*hdma_spi_tx).Init.PeriphInc = DMA_PINC_DISABLE;
    (*hdma_spi_tx).Init.MemInc = DMA_MINC_ENABLE;
    (*hdma_spi_tx).Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    (*hdma_spi_tx).Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    if(spiHandle->Instance == BOARD_COMM_SPI)
    {
        (*hdma_spi_tx).Init.Mode = DMA_NORMAL;
    }
    else
    {
        (*hdma_spi_tx).Init.Mode = DMA_NORMAL;
    }
    (*hdma_spi_tx).Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(hdma_spi_tx) != HAL_OK)
    {
        error_handler(SPI_TX_DMA_INIT_FAILIURE);
    }

    __HAL_LINKDMA(spiHandle,hdmatx,*hdma_spi_tx);

    if(spiHandle->Instance == BOARD_COMM_SPI)
    {
        HAL_NVIC_SetPriority(txDmaIrqn, BOARD_COMM_SPI_DMA_TX_PRE_PRI, BOARD_COMM_SPI_DMA_TX_SUB_PRI);
        HAL_NVIC_EnableIRQ(txDmaIrqn);
    }
    else
    {
        HAL_NVIC_SetPriority(txDmaIrqn, GYRO_SPI_DMA_TX_PRE_PRI, GYRO_SPI_DMA_TX_SUB_PRI);
        HAL_NVIC_EnableIRQ(txDmaIrqn);
    }
}

void gyro_spi_irq_callback(void)
{
    //irq handler
    HAL_SPI_IRQHandler(&gyroSPIHandle);
}