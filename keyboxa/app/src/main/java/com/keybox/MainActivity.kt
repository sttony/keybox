package com.keybox

import android.os.Bundle
import android.view.MenuItem
import androidx.activity.viewModels
import androidx.appcompat.app.ActionBarDrawerToggle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.GravityCompat
import com.google.android.material.navigation.NavigationView
import com.keybox.databinding.ActivityMainBinding
import com.keybox.ui.EntryListFragment
import com.keybox.viewmodel.MainViewModel

class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener {

    private lateinit var binding: ActivityMainBinding
    private val viewModel: MainViewModel by viewModels()
    private val nativeBridge = NativeBridge()
    private var fileHandle: Long = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        setSupportActionBar(binding.toolbar)

        val toggle = ActionBarDrawerToggle(
            this, binding.drawerLayout, binding.toolbar,
            R.string.navigation_drawer_open, R.string.navigation_drawer_close
        )
        binding.drawerLayout.addDrawerListener(toggle)
        toggle.syncState()

        binding.navView.setNavigationItemSelectedListener(this)

        fileHandle = nativeBridge.createCKBFile()

        // Set initial fragment
        if (savedInstanceState == null) {
            supportFragmentManager.beginTransaction()
                .replace(R.id.nav_host_fragment, EntryListFragment())
                .commit()
            binding.navView.setCheckedItem(R.id.nav_passwords)
        }

        binding.fabAdd.setOnClickListener {
            // TODO: Open add entry screen
        }

        // Mock loading entries
        loadMockEntries()
    }

    private fun loadMockEntries() {
        val mockEntries = listOf(
            Entry("Gmail", "https://mail.google.com", "user@gmail.com"),
            Entry("GitHub", "https://github.com", "john_doe")
        )
        viewModel.setEntries(mockEntries)
        viewModel.setFileLoaded(true)
        viewModel.setUnlocked(true)
    }

    override fun onNavigationItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            R.id.nav_passwords -> {
                supportFragmentManager.beginTransaction()
                    .replace(R.id.nav_host_fragment, EntryListFragment())
                    .commit()
            }
            R.id.nav_sync_settings -> {
                // TODO: Open Sync Settings Fragment
            }
            R.id.nav_security -> {
                // TODO: Open Security Fragment
            }
        }
        binding.drawerLayout.closeDrawer(GravityCompat.START)
        return true
    }

    override fun onBackPressed() {
        if (binding.drawerLayout.isDrawerOpen(GravityCompat.START)) {
            binding.drawerLayout.closeDrawer(GravityCompat.START)
        } else {
            super.onBackPressed()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        if (fileHandle != 0L) {
            nativeBridge.destroyCKBFile(fileHandle)
            fileHandle = 0
        }
    }
}
