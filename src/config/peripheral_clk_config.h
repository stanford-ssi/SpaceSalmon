/* Auto-generated config file peripheral_clk_config.h */
#ifndef PERIPHERAL_CLK_CONFIG_H
#define PERIPHERAL_CLK_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <y> ADC Clock Source
// <id> adc_gclk_selection

// <i> Select the clock source for ADC.
#ifndef CONF_GCLK_ADC0_SRC
#define CONF_GCLK_ADC0_SRC GCLK_PCHCTRL_GEN_GCLK0_Val
#endif

/**
 * \def CONF_GCLK_ADC0_FREQUENCY
 * \brief ADC0's Clock frequency
 */
#ifndef CONF_GCLK_ADC0_FREQUENCY
#define CONF_GCLK_ADC0_FREQUENCY 48000000
#endif

// <y> EIC Clock Source
// <id> eic_gclk_selection

// <i> Select the clock source for EIC.
#ifndef CONF_GCLK_EIC_SRC
#define CONF_GCLK_EIC_SRC GCLK_PCHCTRL_GEN_GCLK0_Val
#endif

/**
 * \def CONF_GCLK_EIC_FREQUENCY
 * \brief EIC's Clock frequency
 */
#ifndef CONF_GCLK_EIC_FREQUENCY
#define CONF_GCLK_EIC_FREQUENCY 48000000
#endif

/**
 * \def CONF_CPU_FREQUENCY
 * \brief CPU's Clock frequency
 */
#ifndef CONF_CPU_FREQUENCY
#define CONF_CPU_FREQUENCY 48000000
#endif

// <y> Core Clock Source
// <id> core_gclk_selection

// <i> Select the clock source for CORE.
#ifndef CONF_GCLK_SERCOM0_CORE_SRC
#define CONF_GCLK_SERCOM0_CORE_SRC GCLK_PCHCTRL_GEN_GCLK0_Val
#endif

// <y> Slow Clock Source
// <id> slow_gclk_selection

// <i> Select the slow clock source.
#ifndef CONF_GCLK_SERCOM0_SLOW_SRC
#define CONF_GCLK_SERCOM0_SLOW_SRC GCLK_PCHCTRL_GEN_GCLK1_Val
#endif

/**
 * \def CONF_GCLK_SERCOM0_CORE_FREQUENCY
 * \brief SERCOM0's Core Clock frequency
 */
#ifndef CONF_GCLK_SERCOM0_CORE_FREQUENCY
#define CONF_GCLK_SERCOM0_CORE_FREQUENCY 48000000
#endif

/**
 * \def CONF_GCLK_SERCOM0_SLOW_FREQUENCY
 * \brief SERCOM0's Slow Clock frequency
 */
#ifndef CONF_GCLK_SERCOM0_SLOW_FREQUENCY
#define CONF_GCLK_SERCOM0_SLOW_FREQUENCY 32768
#endif

// <y> Core Clock Source
// <id> core_gclk_selection

// <i> Select the clock source for CORE.
#ifndef CONF_GCLK_SERCOM2_CORE_SRC
#define CONF_GCLK_SERCOM2_CORE_SRC GCLK_PCHCTRL_GEN_GCLK0_Val
#endif

// <y> Slow Clock Source
// <id> slow_gclk_selection

// <i> Select the slow clock source.
#ifndef CONF_GCLK_SERCOM2_SLOW_SRC
#define CONF_GCLK_SERCOM2_SLOW_SRC GCLK_PCHCTRL_GEN_GCLK1_Val
#endif

/**
 * \def CONF_GCLK_SERCOM2_CORE_FREQUENCY
 * \brief SERCOM2's Core Clock frequency
 */
#ifndef CONF_GCLK_SERCOM2_CORE_FREQUENCY
#define CONF_GCLK_SERCOM2_CORE_FREQUENCY 48000000
#endif

/**
 * \def CONF_GCLK_SERCOM2_SLOW_FREQUENCY
 * \brief SERCOM2's Slow Clock frequency
 */
#ifndef CONF_GCLK_SERCOM2_SLOW_FREQUENCY
#define CONF_GCLK_SERCOM2_SLOW_FREQUENCY 32768
#endif

// <y> Core Clock Source
// <id> core_gclk_selection

// <i> Select the clock source for CORE.
#ifndef CONF_GCLK_SERCOM3_CORE_SRC
#define CONF_GCLK_SERCOM3_CORE_SRC GCLK_PCHCTRL_GEN_GCLK0_Val
#endif

// <y> Slow Clock Source
// <id> slow_gclk_selection

// <i> Select the slow clock source.
#ifndef CONF_GCLK_SERCOM3_SLOW_SRC
#define CONF_GCLK_SERCOM3_SLOW_SRC GCLK_PCHCTRL_GEN_GCLK1_Val
#endif

/**
 * \def CONF_GCLK_SERCOM3_CORE_FREQUENCY
 * \brief SERCOM3's Core Clock frequency
 */
#ifndef CONF_GCLK_SERCOM3_CORE_FREQUENCY
#define CONF_GCLK_SERCOM3_CORE_FREQUENCY 48000000
#endif

/**
 * \def CONF_GCLK_SERCOM3_SLOW_FREQUENCY
 * \brief SERCOM3's Slow Clock frequency
 */
#ifndef CONF_GCLK_SERCOM3_SLOW_FREQUENCY
#define CONF_GCLK_SERCOM3_SLOW_FREQUENCY 32768
#endif

// <y> Core Clock Source
// <id> core_gclk_selection

// <i> Select the clock source for CORE.
#ifndef CONF_GCLK_SERCOM5_CORE_SRC
#define CONF_GCLK_SERCOM5_CORE_SRC GCLK_PCHCTRL_GEN_GCLK0_Val
#endif

// <y> Slow Clock Source
// <id> slow_gclk_selection

// <i> Select the slow clock source.
#ifndef CONF_GCLK_SERCOM5_SLOW_SRC
#define CONF_GCLK_SERCOM5_SLOW_SRC GCLK_PCHCTRL_GEN_GCLK1_Val
#endif

/**
 * \def CONF_GCLK_SERCOM5_CORE_FREQUENCY
 * \brief SERCOM5's Core Clock frequency
 */
#ifndef CONF_GCLK_SERCOM5_CORE_FREQUENCY
#define CONF_GCLK_SERCOM5_CORE_FREQUENCY 48000000
#endif

/**
 * \def CONF_GCLK_SERCOM5_SLOW_FREQUENCY
 * \brief SERCOM5's Slow Clock frequency
 */
#ifndef CONF_GCLK_SERCOM5_SLOW_FREQUENCY
#define CONF_GCLK_SERCOM5_SLOW_FREQUENCY 32768
#endif

// <h> SDHC Clock Settings
// <y> SDHC Clock source

// <i> Select the clock source for SDHC.
// <id> sdhc_gclk_selection
#ifndef CONF_GCLK_SDHC0_SRC
#define CONF_GCLK_SDHC0_SRC GCLK_PCHCTRL_GEN_GCLK0_Val
#endif

// <y> SDHC clock slow source

// <i> Select the clock source for SDHC.
// <id> sdhc_slow_gclk_selection
#ifndef CONF_GCLK_SDHC0_SLOW_SRC
#define CONF_GCLK_SDHC0_SLOW_SRC GCLK_PCHCTRL_GEN_GCLK0_Val //TODO: Why is this not slow???
#endif
// </h>

/**
 * \def SDHC FREQUENCY
 * \brief SDHC's Clock frequency
 */
#ifndef CONF_SDHC0_FREQUENCY
#define CONF_SDHC0_FREQUENCY 48000000
#endif

/**
 * \def SDHC FREQUENCY
 * \brief SDHC's Clock slow frequency
 */
#ifndef CONF_SDHC0_SLOW_FREQUENCY
#define CONF_SDHC0_SLOW_FREQUENCY 48000000
#endif

// <y> USB Clock Source
// <id> usb_gclk_selection

// <i> Select the clock source for USB.
#ifndef CONF_GCLK_USB_SRC
#define CONF_GCLK_USB_SRC GCLK_PCHCTRL_GEN_GCLK0_Val

#endif

/**
 * \def CONF_GCLK_USB_FREQUENCY
 * \brief USB's Clock frequency
 */
#ifndef CONF_GCLK_USB_FREQUENCY
#define CONF_GCLK_USB_FREQUENCY 48000000
#endif

// <<< end of configuration section >>>

#endif // PERIPHERAL_CLK_CONFIG_H
