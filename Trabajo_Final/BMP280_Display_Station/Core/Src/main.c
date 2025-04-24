/* USER CODE BEGIN Header */
/**
 * @file main.c
 * @author Ing. Lucas Kirschner
 * @date 2 Apr 2025
 * @brief Programa principal que mide temperatura y presión usando un BMP280,
 *     	muestra los datos en un display HD44780, y maneja errores mediante una FSM.
 *
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>    /**< Manejo de tipos estándar */
#include <stdbool.h>   /**< Manejo de tipos booleanos estándar */
#include "BMP280.h"    /**< Librería para el manejo del sensor BMP280 */
#include "HD44780.h"   /**< Librería para el control del display LCD HD44780 */
#include "DELAY.h"     /**< Librería para funciones de retardo */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**
 * @brief Estados de la máquina de estados principal.
 */
typedef enum
{
    INIT_COMPONENTS, 		/**< Inicialización de periféricos y dispositivos */
    START_MEASUREMENT, 		/**< Inicio de medición del sensor BMP280 */
    WAIT_MEASUREMENT, 		/**< Espera a que finalice la medición */
    PROCESS_DATA, 			/**< Procesamiento de datos del sensor */
    ANALYZE_DATA, 			/**< Análisis de los datos medidos */
    DISPLAY_DATA, 			/**< Actualización del display con datos medidos */
    WAIT_TIME, 				/**< Espera de tiempo entre mediciones (no implementado en esta versión) */
    ERROR_STATE 			/**< Manejo de errores */
} state_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMP_MIN_C		25.0		/**< Temperatura mínima aceptada (°C) */
#define TEMP_MAX_C		30.0		/**< Temperatura máxima aceptada (°C) */

#define DELAY_FSM		1000		/**< Período de actualización de la FSM (ms) */
#define DELAY_LED		250			/**< Período de parpadeo del LED en estado de error (ms) */
#define DELAY_REINIT	2000		/**< Tiempo de espera para reintentar inicialización tras un error (ms) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN PV */

bmp280_t bmp;					/**< Estructura de datos del sensor BMP280 */
state_t  state;					/**< Estado actual de la máquina de estados */
delay_t  delayFSM;				/**< Temporizador para el control de la FSM */
delay_t  delayLED;				/**< Temporizador para el parpadeo del LED */
delay_t  delayReinit;			/**< Temporizador para reintento de inicialización */
bool 	 tempOutRange;			/**< Bandera que indica si la temperatura está fuera de rango) */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/** @brief Inicializa la máquina de estados */
static void FSM_Init(void);

/** @brief Actualiza la lógica de la máquina de estados */
static void FSM_Update(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  FSM_Init();			/**< Inicializa la máquina de estados */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	FSM_Update();		/**< Actualiza la máquina de estados */

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI3_CS_Pin */
  GPIO_InitStruct.Pin = SPI3_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SPI3_CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief Inicializa los módulos de delay y establece el estado inicial de la FSM.
 */
static void FSM_Init(void)
{
	  Delay_Init(&delayFSM, DELAY_FSM);
	  Delay_Init(&delayLED, DELAY_LED);
	  Delay_Init(&delayReinit, DELAY_REINIT);

	  tempOutRange = false;					/**< Condición inicial de medición fuera de rango */
	  state = INIT_COMPONENTS; 				/**< Estado inicial de la máquina de estados */
}

/**
 * @brief Actualiza la máquina de estados en función del estado actual y los eventos.
 */
static void FSM_Update(void)
{
	switch (state)
	{
	case INIT_COMPONENTS:					/**< Inicicalización de periféricos y dispositivos */
		if(BMP280_Init() != BMP280_OK)		/**< Inicialización del sensor BMP280 e interfaz SPI */
		{
			state = ERROR_STATE;
			break;
		}

		if(HD44780_Init() != HD44780_OK)	/**< Inicialización del display HD44780 e interfaz I2C */
		{
			state = ERROR_STATE;
			break;
		}

		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, true);	/**< Led verde inicia encendido, toggle indica error */

		state = START_MEASUREMENT;			/**< Si salió bien, paso al siguiente estado */
		break;

	case START_MEASUREMENT:					/**< Le indica al BMP280 que inicie una medición */
		if(BMP280_Trigger_Measurement() != BMP280_OK)
		{
			state = ERROR_STATE;
			break;
		}
		state = WAIT_MEASUREMENT;
		break;

	case WAIT_MEASUREMENT:					/**< Espera a que el BMP280 finalice la medición */
		bmp280_status_t status = BMP280_Is_Measuring();
		if(status == BMP280_DATA_RDY)
			state = PROCESS_DATA;
		else if(status != BMP280_DATA_NOT_RDY)
			state = ERROR_STATE;
		break;

	case PROCESS_DATA:						/**< Lee los datos crudos y los compensa */
		if(BMP280_Update_Parameters(&bmp) != BMP280_OK)
		{
			state = ERROR_STATE;
			break;
		}
		state = ANALYZE_DATA;
		break;

	case ANALYZE_DATA:						/**< Analiza si la temperatura esta fuera de rango */
		if(bmp.temperature < TEMP_MIN_C || bmp.temperature > TEMP_MAX_C)
			tempOutRange = true;
		else
			tempOutRange = false;
		state = DISPLAY_DATA;
		break;

	case DISPLAY_DATA:						/**< Actualiza los datos del display */
		if(HD44780_Set_Cursor(1,1) != HD44780_OK)
		{
			state = ERROR_STATE;
			break;
		}
		if(HD44780_Write((uint8_t *)"Temp: ") != HD44780_OK)
		{
			state = ERROR_STATE;
			break;
		}
		if(HD44780_Write_int((int16_t) bmp.temperature) != HD44780_OK)
		{
			state = ERROR_STATE;
			break;
		}
		if(HD44780_Write((uint8_t *)" C") != HD44780_OK)
		{
			state = ERROR_STATE;
			break;
		}
		if(tempOutRange)	/**< Advertencia */
		{
			if(HD44780_Write((uint8_t *)" (!)") != HD44780_OK)
			{
				state = ERROR_STATE;
				break;
			}
		}
		else				/**< Limpio display */
		{
			if(HD44780_Write((uint8_t *)"    ") != HD44780_OK)
			{
				state = ERROR_STATE;
				break;
			}
		}

		if(HD44780_Set_Cursor(2,1) != HD44780_OK)
		{
			state = ERROR_STATE;
			break;
		}
		if(HD44780_Write((uint8_t *)"Pres: ") != HD44780_OK)
		{
			state = ERROR_STATE;
			break;
		}
		if(HD44780_Write_int((int16_t) bmp.pressure) != HD44780_OK)
		{
			state = ERROR_STATE;
			break;
		}
		if(HD44780_Write((uint8_t *)" hPa") != HD44780_OK)
		{
			state = ERROR_STATE;
			break;
		}

		state = WAIT_TIME;

		break;

	case WAIT_TIME:						/**< Espera un tiempo para actualizar la medición (repite el ciclo) */
		if(Delay_Read(&delayFSM))
			state = START_MEASUREMENT;
		break;

	case ERROR_STATE:					/**< Se ejecuta si ocurre cualquier error */
		if(Delay_Read(&delayLED))		/**< Toggle led verde de la Nucleo */
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		if(Delay_Read(&delayReinit))	/* Luego de cierto tiempo, intenta reiniciar */
			state = INIT_COMPONENTS;
		break;

	default:
		FSM_Init();
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
