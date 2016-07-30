
/* Standard includes. */
#include <stdio.h>
#include <string.h>

#include	"BasicWEB_Task.h"
#include	"Net_Data.h"
#include	"MyMem.h"
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include	<string.h>

#include "tcpip.h"


/*! The size of the buffer in which the dynamic WEB page is created. */
#define webMAX_PAGE_SIZE	100

/*! Standard GET response. */
#define webHTTP_OK	"HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"

/*! The port on which we listen. */
#define webHTTP_PORT		( 80 )


/*! Format of the dynamic page that is returned on each connection. */
#define webHTML_START \
"<html>\
<head>\
</head>\
<BODY>\
"

#define webHTML_END \
"</BODY>\
</html>"


/*! Function to process the current connection */
static void prvweb_ParseHTMLRequest( struct netconn *pxNetCon );
static void vBasicWEBServer( void *pvParameters );

void StartBasicWebTask(void)
{
	xTaskCreate( vBasicWEBServer, ( const char * ) "WEB	", configMINIMAL_STACK_SIZE*2, NULL, 2,  NULL );
}

/*! \brief WEB server main task
 *         check for incoming connection and process it
 *
 *  \param pvParameters   Input. Not Used.
 *
 */

static void vBasicWEBServer( void *pvParameters )
{
	struct netconn *pxHTTPListener, *pxNewConnection;
	err_t err;
	/* Create a new tcp connection handle */
	pxHTTPListener = netconn_new( NETCONN_TCP );
	
	netconn_bind(pxHTTPListener, IP_ADDR_ANY, webHTTP_PORT );
	netconn_listen( pxHTTPListener );

	/* Loop forever */
	for( ;; )
	{
		/* Wait for a first connection. */
		 err = netconn_accept(pxHTTPListener, &pxNewConnection);

		if(err == ERR_OK)
		{
			/* connection timeout 5000 ms */
			pxNewConnection->recv_timeout = (1000 * portTICK_RATE_MS);
			prvweb_ParseHTMLRequest(pxNewConnection);
		}/* end if new connection */

	} /* end infinite loop */
}
//------------------------------------------------------------------------------------------
/*! \brief parse the incoming request
 *         parse the HTML request and send file
 *
 *  \param pxNetCon   Input. The netconn to use to send and receive data.
 *
 */
static void prvweb_ParseHTMLRequest( struct netconn *pxNetCon )
{
	char * cDynamicPage = NULL;
	portCHAR *pcRxString;
	unsigned portSHORT usLength;
	struct netbuf *pxRxBuffer;
	err_t err;

	/* We expect to immediately get data. */
	err = netconn_recv( pxNetCon, &pxRxBuffer);
	
	if(err == ERR_OK)
	{
		/* Where is the data? */
		netbuf_data( pxRxBuffer, ( void * ) &pcRxString, &usLength );

		/* Is this a GET?  We don't handle anything else. */
		if(( NULL != pcRxString               )
		 && ( !strncmp( pcRxString, "GET", 3 ) ))
		{
			cDynamicPage = MyMalloc(2000);
			if(cDynamicPage)
			{
				memset(cDynamicPage, 0, 2000);
				/* Write out the HTTP OK header. */
				netconn_write( pxNetCon, webHTTP_OK, (u16_t) strlen( webHTTP_OK ), NETCONN_COPY );
				
				/* Generate the dynamic page... First the page header. */
				strcpy( cDynamicPage, webHTML_START );

				strcat( cDynamicPage, "<p><pre>Task            State   Priority    Stack	#<br>************************************************<br>" );

				/* ... Then the list of tasks and their status... */
				//vTaskList(cDynamicPage + strlen( cDynamicPage ) );

				if(Link_Up == GetGB_WifiState())
					strcat( cDynamicPage, "Wifi_LinkUP<br>" );
				else
					strcat( cDynamicPage, "Wifi_LinkDown<br>" );
				
				if(Link_Up == GetGB_LinkState())
					strcat( cDynamicPage, "Net_LinkUP<br>" );
				else
					strcat( cDynamicPage, "Net_LinkDown<br>" );
				
				if(NetNone == GetGB_NetCard())
					strcat( cDynamicPage, "NetNone<br>" );
				else if(Line_Mode == GetGB_NetCard())
					strcat( cDynamicPage, "Line_Mode<br>" );
				else
					strcat( cDynamicPage, "Wifi_Mode<br>" );
				/* ... Finally the page footer. */
				strcat( cDynamicPage, webHTML_END );

				/* Write out the dynamically generated page. */
				netconn_write( pxNetCon, cDynamicPage, (u16_t) strlen( cDynamicPage ), NETCONN_COPY );
			}
			MyFree(cDynamicPage);
		}
		netbuf_delete( pxRxBuffer );
	}

	netconn_close( pxNetCon );
	netconn_delete( pxNetCon );
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
