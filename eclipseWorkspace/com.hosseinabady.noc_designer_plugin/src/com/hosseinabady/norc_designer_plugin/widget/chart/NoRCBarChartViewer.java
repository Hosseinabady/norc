package com.hosseinabady.norc_designer_plugin.widget.chart;


import org.eclipse.birt.chart.api.ChartEngine;
import org.eclipse.birt.chart.device.IDeviceRenderer;
import org.eclipse.birt.chart.exception.ChartException;
import org.eclipse.birt.chart.factory.GeneratedChartState;
import org.eclipse.birt.chart.factory.Generator;
import org.eclipse.birt.chart.model.Chart;
import org.eclipse.birt.chart.model.ChartWithAxes;
import org.eclipse.birt.chart.model.attribute.AxisType;
import org.eclipse.birt.chart.model.attribute.Bounds;
import org.eclipse.birt.chart.model.attribute.IntersectionType;
import org.eclipse.birt.chart.model.attribute.LineAttributes;
import org.eclipse.birt.chart.model.attribute.LineStyle;
import org.eclipse.birt.chart.model.attribute.Marker;
import org.eclipse.birt.chart.model.attribute.MarkerType;
import org.eclipse.birt.chart.model.attribute.Position;
import org.eclipse.birt.chart.model.attribute.RiserType;
import org.eclipse.birt.chart.model.attribute.TickStyle;
import org.eclipse.birt.chart.model.attribute.impl.BoundsImpl;
import org.eclipse.birt.chart.model.attribute.impl.ColorDefinitionImpl;
import org.eclipse.birt.chart.model.component.Axis;
import org.eclipse.birt.chart.model.component.Series;
import org.eclipse.birt.chart.model.component.impl.SeriesImpl;
import org.eclipse.birt.chart.model.data.NumberDataSet;
import org.eclipse.birt.chart.model.data.SeriesDefinition;
import org.eclipse.birt.chart.model.data.impl.NumberDataSetImpl;
import org.eclipse.birt.chart.model.data.impl.SeriesDefinitionImpl;
import org.eclipse.birt.chart.model.impl.ChartWithAxesImpl;
import org.eclipse.birt.chart.model.layout.Legend;
import org.eclipse.birt.chart.model.layout.Plot;
import org.eclipse.birt.chart.model.type.BarSeries;
import org.eclipse.birt.chart.model.type.LineSeries;
import org.eclipse.birt.chart.model.type.impl.BarSeriesImpl;
import org.eclipse.birt.chart.model.type.impl.LineSeriesImpl;
import org.eclipse.birt.core.framework.PlatformConfig;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;

import com.hosseinabady.norc_designer_plugin.data.NoRCTrafficPowerDataSet;
import com.hosseinabady.norc_designer_plugin.editors.NoRCChartTypeConstants;
public class NoRCBarChartViewer extends Composite implements PaintListener, ControlListener
{
	private IDeviceRenderer idr = null;
	private Chart cmTraffic = null;
	private Chart cmPower = null;

	private GeneratedChartState gcs = null;
	private GeneratedChartState gcs2 = null;
	public static NoRCBarChartViewer c3dViewer;
	public static  double[] sa;
	public static double[] da1;
	public static double[] da2;

	/**
	 * Used in building the chart for the first time
	 */
	private boolean bFirstPaint = true;

	/**
	 * execute application
	 * 
	 * @param args
	 */
	@Override
	public void dispose() {
		if( (imgChart !=null) && (!imgChart.isDisposed()))imgChart.dispose();
		if( (gcImage !=null) && (!gcImage.isDisposed()))gcImage.dispose();
		super.dispose();
	}
	
	/**
	 * Constructor
	 */
	public NoRCBarChartViewer( Composite parent, int style, NoRCTrafficPowerDataSet dataSet, int type )
	{
		super( parent, style );
		try
		{
			int[] intSa =dataSet.getIds();
			sa = new double[intSa.length];
			for (int i = 0; i < intSa.length; i++)
				sa[i] =  intSa[i];
			if (type == NoRCChartTypeConstants.TRAFFIC)
				da1 = dataSet.getTraffics();
			else if (type == NoRCChartTypeConstants.POWER)
				da1 = dataSet.getPowers();
			PlatformConfig config = new PlatformConfig( );
			config.setBIRTHome("");
			idr = ChartEngine.instance( config ).getRenderer( "dv.SWT" );
		}
		catch ( ChartException pex )
		{
			pex.printStackTrace();
		}
		cmTraffic = createChart(type); 
		
	}
	public static final Chart createChart(int type)
	{
		ChartWithAxes cwaBar = ChartWithAxesImpl.create( );

		// Plot
		cwaBar.getBlock( ).setBackground( ColorDefinitionImpl.WHITE( ) );
		Plot p = cwaBar.getPlot( );
		p.getClientArea( ).setBackground( ColorDefinitionImpl.create( 255,
				255,
				225 ) );
		// Legend
		Legend lg = cwaBar.getLegend( );
		LineAttributes lia = lg.getOutline( );
		lg.getText( ).getFont( ).setSize( 16 );
		lia.setStyle( LineStyle.SOLID_LITERAL );
		lg.getInsets( ).setLeft( 10 );
		lg.getInsets( ).setRight( 10 );
		// Title
		if (type == NoRCChartTypeConstants.TRAFFIC)
			cwaBar.getTitle( )
			.getLabel( )
			.getCaption( )
			.setValue( "Router Traffic Profiling" );//$NON-NLS-1$
		else if (type == NoRCChartTypeConstants.POWER)
			cwaBar.getTitle( )
			.getLabel( )
			.getCaption( )
			.setValue( "Router Power Profiling" );//$NON-NLS-1$
		
		// X-Axis
		Axis xAxisPrimary = cwaBar.getPrimaryBaseAxes( )[0];
		xAxisPrimary.setType( AxisType.TEXT_LITERAL );
		xAxisPrimary.getOrigin( ).setType( IntersectionType.VALUE_LITERAL );
		xAxisPrimary.getOrigin( ).setType( IntersectionType.MIN_LITERAL );
		xAxisPrimary.getTitle( )
		.getCaption( )
		.setValue( "Category Text X-Axis" );//$NON-NLS-1$
		xAxisPrimary.setTitlePosition( Position.BELOW_LITERAL );
		xAxisPrimary.getLabel( ).getCaption( ).getFont( ).setRotation( 75 );
		xAxisPrimary.setLabelPosition( Position.BELOW_LITERAL );
		xAxisPrimary.getMajorGrid( ).setTickStyle( TickStyle.BELOW_LITERAL );
		xAxisPrimary.getMajorGrid( )
		.getLineAttributes( )
		.setStyle( LineStyle.DOTTED_LITERAL );
		xAxisPrimary.getMajorGrid( )
		.getLineAttributes( )
		.setColor( ColorDefinitionImpl.create( 64, 64, 64 ) );
		xAxisPrimary.getMajorGrid( ).getLineAttributes( ).setVisible( true );
		// Y-Axis
		Axis yAxisPrimary = cwaBar.getPrimaryOrthogonalAxis( xAxisPrimary );
		yAxisPrimary.getLabel( ).getCaption( ).setValue( "Price Axis" );//$NON-NLS-1$
		yAxisPrimary.getLabel( ).getCaption( ).getFont( ).setRotation( 37 );
		yAxisPrimary.setLabelPosition( Position.LEFT_LITERAL );
		yAxisPrimary.setTitlePosition( Position.LEFT_LITERAL );
		yAxisPrimary.getTitle( ).getCaption( ).setValue( "Linear Value Y-Axis" );//$NON-NLS-1$
		yAxisPrimary.setType( AxisType.LINEAR_LITERAL );
		yAxisPrimary.getMajorGrid( ).setTickStyle( TickStyle.LEFT_LITERAL );
		yAxisPrimary.getMajorGrid( )
		.getLineAttributes( )
		.setStyle( LineStyle.DOTTED_LITERAL );
		yAxisPrimary.getMajorGrid( )
		.getLineAttributes( )
		.setColor( ColorDefinitionImpl.RED( ) );
		yAxisPrimary.getMajorGrid( ).getLineAttributes( ).setVisible( true );
		// X-Series
		Series seCategory = SeriesImpl.create( );
		SeriesDefinition sdX = SeriesDefinitionImpl.create( );
		xAxisPrimary.getSeriesDefinitions( ).add( sdX );
		sdX.getSeries( ).add( seCategory );
		// Y-Series (1)
		BarSeries bs1 = (BarSeries) BarSeriesImpl.create( );
		if (type == NoRCChartTypeConstants.TRAFFIC)
			bs1.setSeriesIdentifier( "# of Phits" );//$NON-NLS-1$
		else if (type == NoRCChartTypeConstants.POWER)
			bs1.setSeriesIdentifier( "Power" );//$NON-NLS-1$
		
		bs1.setRiserOutline( null );
		bs1.setRiser( RiserType.RECTANGLE_LITERAL );
		// Y-Series (2)
		LineSeries ls1 = (LineSeries) LineSeriesImpl.create( );
		ls1.setSeriesIdentifier( "POWER" );//$NON-NLS-1$
		ls1.getLineAttributes( ).setColor( ColorDefinitionImpl.BLUE());
		for ( int i = 0; i < ls1.getMarkers( ).size( ); i++ )
		{
			( (Marker) ls1.getMarkers( ).get( i ) ).setType( MarkerType.BOX_LITERAL );
		}
		ls1.setCurve( true );
		SeriesDefinition sdY = SeriesDefinitionImpl.create( );
		yAxisPrimary.getSeriesDefinitions( ).add( sdY );
		sdY.getSeriesPalette( ).shift( -1 );
		sdY.getSeries( ).add( bs1 );
		sdY.getSeries( ).add( ls1 );
		// Update data
		updateDataSet( cwaBar);
		return cwaBar;
	}
	static final void updateDataSet( ChartWithAxes cwaBar)
	{
		// Associate with Data Set

		NumberDataSet categoryValues = NumberDataSetImpl.create( sa );
		NumberDataSet seriesOneValues = NumberDataSetImpl.create( da1 );
		NumberDataSet seriesTwoValues = NumberDataSetImpl.create( da2 );
		// X-Axis
		Axis xAxisPrimary = cwaBar.getPrimaryBaseAxes( )[0];
		SeriesDefinition sdX = (SeriesDefinition) xAxisPrimary.getSeriesDefinitions( )
		.get( 0 );
		( (Series) sdX.getSeries( ).get( 0 ) ).setDataSet( categoryValues );
		// Y-Axis
		Axis yAxisPrimary = cwaBar.getPrimaryOrthogonalAxis( xAxisPrimary );
		SeriesDefinition sdY = (SeriesDefinition) yAxisPrimary.getSeriesDefinitions( )
		.get( 0 );
		( (Series) sdY.getSeries( ).get( 0 ) ).setDataSet( seriesOneValues );
		//( (Series) sdY.getSeries( ).get( 1 ) ).setDataSet( seriesTwoValues );
	
	}
	// Live Date Set
	/*private static final String[] sa = {
		"One", "Two", "Three", "Four", "Five",//$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$//$NON-NLS-4$//$NON-NLS-5$
		"Six", "Seven", "Eight", "Nine", "Ten"};//$NON-NLS-1$//$NON-NLS-2$//$NON-NLS-3$//$NON-NLS-4$//$NON-NLS-5$*/


	private Image imgChart;
	
	private GC gcImage;
	
	private Bounds bo;
/*	static final void scrollData( ChartWithAxes cwa )
	{
		// Scroll the bar (Y) series
		double dTemp = da1[0];
		for ( int i = 0; i < da1.length - 1; i++ )
		{
			da1[i] = da1[i + 1];
		}
		da1[da1.length - 1] = dTemp;
		// Scroll the line (Y) series
		dTemp = da2[0];
		for ( int i = 0; i < da2.length - 1; i++ )
		{
			da2[i] = da2[i + 1];
		}
		da2[da2.length - 1] = dTemp;
		// Scroll X series
		//String sTemp = sa[0];
		double sTemp = sa[0];
		for ( int i = 0; i < sa.length - 1; i++ )
		{
			sa[i] = sa[i + 1];
		}
		sa[sa.length - 1] = sTemp;
		updateDataSet( cwa, type );
	}*/
	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.swt.events.PaintListener#paintControl(org.eclipse.swt.events
	 * .PaintEvent)
	 */
	public final void paintControl( PaintEvent e )
	{
		Rectangle d = this.getClientArea( );
		if ( bFirstPaint )
		{
			//if resized
			if( (imgChart !=null) && (!imgChart.isDisposed()))imgChart.dispose();
			if( (gcImage !=null) && (!gcImage.isDisposed()))gcImage.dispose();
			
			imgChart = new Image( this.getDisplay( ), d );
			gcImage = new GC( imgChart );
			idr.setProperty( IDeviceRenderer.GRAPHICS_CONTEXT, gcImage );
			bo = BoundsImpl.create( 0, 0, d.width, d.height );
			bo.scale( 72d / idr.getDisplayServer( ).getDpiResolution( ) );
		}
		Generator grTraffic = Generator.instance( );
		
		try
		{
			gcs = grTraffic.build( idr.getDisplayServer( ), cmTraffic, bo, null, null, null );
			grTraffic.render( idr, gcs );
			
			
			GC gc = e.gc;
			gc.drawImage( imgChart, d.x, d.y );
			
			
		}
		catch ( ChartException ce )
		{
			ce.printStackTrace( );
		}
		bFirstPaint = false;
		Display.getDefault( ).timerExec( 500, new Runnable( ) {
			public void run( )
			{
				//-----  MOHAMMAD START
				//-----  ADDED TO FIX THE CHART
				//chartRefresh( );
				//----- MOHAMMAD END
			}
		} );
	}
	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse
	 * .swt.events.SelectionEvent)
	 */
	private void chartRefresh( )
	{
		if ( !isDisposed( ) )
		{
			final Generator gr = Generator.instance( );
			//scrollData( (ChartWithAxes) cmTraffic );
			// Refresh
			try
			{
				gr.refresh( gcs );
			}
			catch ( ChartException ex )
			{
				ex.printStackTrace( );
			}
			redraw( );
		}
	}
	public void controlMoved(ControlEvent arg0) {
		bFirstPaint = true;

	}
	public void controlResized(ControlEvent arg0) {
		bFirstPaint = true;
	}
	public void setData(){
		
	}
}