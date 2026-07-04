package com.keybox

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.view.MenuItem
import android.view.View
import android.widget.EditText
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AlertDialog
import androidx.activity.viewModels
import androidx.appcompat.app.ActionBarDrawerToggle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import androidx.core.view.GravityCompat
import com.google.android.material.navigation.NavigationView
import com.keybox.databinding.ActivityMainBinding
import com.keybox.ui.ChangePasswordFragment
import com.keybox.ui.EntryEditorFragment
import com.keybox.ui.EntryListFragment
import com.keybox.ui.GroupsFragment
import com.keybox.ui.PasswordGeneratorFragment
import com.keybox.ui.SyncSettingsFragment
import com.keybox.ui.UnlockFragment
import com.keybox.viewmodel.MainViewModel
import java.io.File
import java.security.SecureRandom

class MainActivity : AppCompatActivity(), NavigationView.OnNavigationItemSelectedListener {

    private lateinit var binding: ActivityMainBinding
    private val viewModel: MainViewModel by viewModels()
    internal var kbFile = KBFile()
        private set
    private val keyboxFile: File by lazy { File(filesDir, KEYBOX_FILE_NAME) }
    private var loadedFileData: ByteArray? = null
    private var drawerGroups: List<Group> = emptyList()
    private var isSyncing = false
    private var pendingExportData: ByteArray? = null

    private val exportLauncher = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        val uri = result.data?.data
        if (result.resultCode == RESULT_OK && uri != null) {
            writeExportedKeybox(uri)
        }
    }

    private val importLauncher = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        val uri = result.data?.data
        if (result.resultCode == RESULT_OK && uri != null) {
            readImportedKeybox(uri)
        }
    }

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
        val toolbarForeground = ContextCompat.getColor(this, R.color.toolbar_foreground)
        binding.toolbar.setTitleTextColor(toolbarForeground)
        toggle.drawerArrowDrawable.color = toolbarForeground

        binding.navView.setNavigationItemSelectedListener(this)

        viewModel.isUnlocked.observe(this) { unlocked ->
            binding.fabAdd.visibility = if (unlocked) View.VISIBLE else View.GONE
        }

        if (savedInstanceState == null) {
            supportFragmentManager.beginTransaction()
                .replace(R.id.nav_host_fragment, UnlockFragment())
                .commit()
        }

        binding.fabAdd.setOnClickListener {
            if (viewModel.isUnlocked.value == true) {
                openEntryEditor()
            }
        }

        configureNativeCaBundle()
        initializeKeyboxFile()
    }

    private fun configureNativeCaBundle() {
        val caBundleFile = File(filesDir, CA_BUNDLE_FILE_NAME)
        if (!caBundleFile.exists()) {
            resources.openRawResource(R.raw.cacert).use { input ->
                caBundleFile.outputStream().use { output ->
                    input.copyTo(output)
                }
            }
        }
        kbFile.setCurlCaBundlePath(caBundleFile.absolutePath)
    }

    private fun initializeKeyboxFile() {
        viewModel.setLoadError(null)
        if (keyboxFile.exists()) {
            val data = runCatching { keyboxFile.readBytes() }
                .getOrElse {
                    viewModel.setLoadError(getString(R.string.load_keybox_failed, it.localizedMessage ?: "unknown error"))
                    return
                }
            loadedFileData = data
            val result = kbFile.loadHeader(data)
            if (result == 0) {
                viewModel.setFileLoaded(true)
                viewModel.setPendingNewFile(false)
                viewModel.setUnlocked(false)
            } else {
                viewModel.setLoadError(getString(R.string.load_keybox_error_code, result))
                viewModel.setFileLoaded(false)
            }
        } else {
            val salt = ByteArray(KEY_DERIVATION_SALT_BYTES)
            SecureRandom().nextBytes(salt)
            val result = kbFile.setDerivativeParameters(salt, KEY_DERIVATION_ROUNDS)
            if (result == 0) {
                viewModel.setFileLoaded(true)
                viewModel.setPendingNewFile(true)
                viewModel.setUnlocked(false)
            } else {
                viewModel.setLoadError(getString(R.string.create_keybox_error_code, result))
                viewModel.setFileLoaded(false)
            }
        }
    }

    internal fun unlockExistingKeybox(masterPassword: String): Boolean {
        val data = loadedFileData ?: runCatching { keyboxFile.readBytes() }
            .onSuccess { loadedFileData = it }
            .getOrElse {
                viewModel.setLoadError(getString(R.string.load_keybox_failed, it.localizedMessage ?: "unknown error"))
                return false
            }

        viewModel.setLoadError(null)
        val passwordResult = kbFile.setMasterPassword(masterPassword)
        if (passwordResult != 0) {
            viewModel.setLoadError(getString(R.string.unlock_keybox_error_code, passwordResult))
            return false
        }

        val payloadResult = kbFile.loadPayload(data)
        if (payloadResult != 0) {
            viewModel.setLoadError(getString(R.string.password_incorrect_or_file_corrupt))
            return false
        }

        loadPayloadIntoViewModel()
        showEntryList()
        return true
    }

    internal fun createNewKeyboxWithPassword(masterPassword: String): Boolean {
        viewModel.setLoadError(null)
        val passwordResult = kbFile.setMasterPassword(masterPassword)
        if (passwordResult != 0) {
            viewModel.setLoadError(getString(R.string.create_keybox_error_code, passwordResult))
            return false
        }

        if (!saveKeyboxFile()) {
            return false
        }

        loadedFileData = keyboxFile.readBytes()
        viewModel.setPendingNewFile(false)
        loadPayloadIntoViewModel()
        showEntryList()
        return true
    }

    internal fun loadDownloadedKeybox(data: ByteArray, masterPassword: String): Boolean {
        val candidate = KBFile()
        val headerResult = candidate.loadHeader(data)
        if (headerResult != 0) {
            candidate.destroy()
            viewModel.setLoadError(getString(R.string.load_keybox_error_code, headerResult))
            return false
        }

        val passwordResult = candidate.setMasterPassword(masterPassword)
        if (passwordResult != 0) {
            candidate.destroy()
            viewModel.setLoadError(getString(R.string.unlock_keybox_error_code, passwordResult))
            return false
        }

        val payloadResult = candidate.loadPayload(data)
        if (payloadResult != 0) {
            candidate.destroy()
            viewModel.setLoadError(getString(R.string.password_incorrect_or_file_corrupt))
            return false
        }

        kbFile.destroy()
        kbFile = candidate
        loadedFileData = data
        if (!saveKeyboxFile()) {
            return false
        }
        loadPayloadIntoViewModel()
        showEntryList()
        return true
    }

    private fun loadPayloadIntoViewModel() {
        val groups = kbFile.getGroups()
        viewModel.setEntries(kbFile.getEntries())
        viewModel.setGroups(groups)
        viewModel.setFileLoaded(true)
        viewModel.setUnlocked(true)
        updateDrawerGroups(groups)
    }

    private fun showEntryList() {
        supportFragmentManager.popBackStack(null, androidx.fragment.app.FragmentManager.POP_BACK_STACK_INCLUSIVE)
        supportFragmentManager.beginTransaction()
            .replace(R.id.nav_host_fragment, EntryListFragment())
            .commit()
        binding.navView.setCheckedItem(R.id.nav_passwords)
    }

    private fun openEntryEditor() {
        supportFragmentManager.beginTransaction()
            .replace(R.id.nav_host_fragment, EntryEditorFragment.newInstance())
            .addToBackStack(null)
            .commit()
    }

    internal fun addEntryAndRefresh(entry: Entry): Int {
        val result = kbFile.addEntry(entry)
        if (result == 0) {
            saveKeyboxFile()
            refreshEntriesFromFile()
        }
        return result
    }

    internal fun removeEntryAndRefresh(entryId: String): Int {
        val result = kbFile.removeEntry(entryId)
        if (result == 0) {
            saveKeyboxFile()
            refreshEntriesFromFile()
        }
        return result
    }

    internal fun updateEntryAndRefresh(entry: Entry): Int {
        val result = kbFile.updateEntry(entry)
        if (result == 0) {
            saveKeyboxFile()
            refreshEntriesFromFile()
        }
        return result
    }

    internal fun getEntryAttachment(entryId: String): ByteArray? {
        return kbFile.getEntryAttachment(entryId)
    }

    internal fun updateGroupAndRefresh(groupId: String, name: String): Int {
        val result = kbFile.updateGroup(groupId, name)
        if (result == 0) {
            saveKeyboxFile()
            refreshEntriesFromFile()
        }
        return result
    }

    internal fun removeGroupAndRefresh(groupId: String): Int {
        val result = kbFile.removeGroup(groupId)
        if (result == 0) {
            if (viewModel.selectedGroup.value?.id == groupId) {
                viewModel.setSelectedGroup(null)
            }
            saveKeyboxFile()
            refreshEntriesFromFile()
        }
        return result
    }

    internal fun refreshEntriesFromFile() {
        val groups = kbFile.getGroups()
        viewModel.setEntries(kbFile.getEntries())
        viewModel.setGroups(groups)
        updateDrawerGroups(groups)
    }

    internal fun saveKeyboxFile(): Boolean {
        val data = kbFile.serialize()
        if (data == null) {
            viewModel.setLoadError(getString(R.string.save_keybox_failed))
            return false
        }
        return runCatching {
            keyboxFile.writeBytes(data)
            loadedFileData = data
        }.onFailure {
            viewModel.setLoadError(getString(R.string.save_keybox_failed_with_message, it.localizedMessage ?: "unknown error"))
        }.isSuccess
    }

    override fun onNavigationItemSelected(item: MenuItem): Boolean {
        if (viewModel.isUnlocked.value != true && item.itemId != R.id.nav_passwords) {
            binding.drawerLayout.closeDrawer(GravityCompat.START)
            return true
        }
        if (item.groupId == DRAWER_GROUP_MENU_GROUP_ID) {
            val group = drawerGroups.getOrNull(item.itemId - DRAWER_GROUP_MENU_ID_BASE)
            viewModel.setSelectedGroup(group)
            showEntryList()
            binding.drawerLayout.closeDrawer(GravityCompat.START)
            return true
        }
        when (item.itemId) {
            R.id.nav_passwords -> {
                if (viewModel.isUnlocked.value == true) {
                    viewModel.setSelectedGroup(null)
                    supportFragmentManager.beginTransaction()
                        .replace(R.id.nav_host_fragment, EntryListFragment())
                        .commit()
                }
            }
            R.id.nav_sync_settings -> {
                supportFragmentManager.beginTransaction()
                    .replace(R.id.nav_host_fragment, SyncSettingsFragment())
                    .addToBackStack(null)
                    .commit()
            }
            R.id.nav_sync_now -> {
                syncNow()
            }
            R.id.nav_password_generator -> {
                supportFragmentManager.beginTransaction()
                    .replace(R.id.nav_host_fragment, PasswordGeneratorFragment())
                    .addToBackStack(null)
                    .commit()
            }
            R.id.nav_manage_groups -> {
                supportFragmentManager.beginTransaction()
                    .replace(R.id.nav_host_fragment, GroupsFragment())
                    .addToBackStack(null)
                    .commit()
            }
            R.id.nav_import_keybox -> {
                importKeybox()
            }
            R.id.nav_export_keybox -> {
                exportKeybox()
            }
            R.id.nav_security -> {
                supportFragmentManager.beginTransaction()
                    .replace(R.id.nav_host_fragment, ChangePasswordFragment())
                    .addToBackStack(null)
                    .commit()
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
        kbFile.destroy()
    }

    override fun onStop() {
        if (viewModel.isUnlocked.value == true) {
            saveKeyboxFile()
        }
        super.onStop()
    }

    private companion object {
        const val KEYBOX_FILE_NAME = "keybox.kbx"
        const val CA_BUNDLE_FILE_NAME = "cacert.pem"
        const val KEY_DERIVATION_SALT_BYTES = 32
        const val KEY_DERIVATION_ROUNDS = 60000
        const val DRAWER_GROUP_MENU_GROUP_ID = 10
        const val DRAWER_GROUP_MENU_ID_BASE = 10_000
    }

    private fun updateDrawerGroups(groups: List<Group>) {
        drawerGroups = groups
        val menu = binding.navView.menu
        menu.removeGroup(DRAWER_GROUP_MENU_GROUP_ID)
        if (groups.isEmpty()) return

        menu.add(DRAWER_GROUP_MENU_GROUP_ID, 0, 0, R.string.groups)
            .isEnabled = false
        groups.forEachIndexed { index, group ->
            menu.add(
                DRAWER_GROUP_MENU_GROUP_ID,
                DRAWER_GROUP_MENU_ID_BASE + index,
                index,
                group.name
            ).apply {
                setIcon(android.R.drawable.ic_menu_upload)
                isCheckable = true
            }
        }
    }

    private fun syncNow() {
        if (isSyncing) return
        setSyncInProgress(true)

        Thread {
            val result = runCatching { kbFile.pushToRemote() }
            runOnUiThread {
                setSyncInProgress(false)
                result.onSuccess { message ->
                    saveKeyboxFile()
                    AlertDialog.Builder(this)
                        .setTitle(R.string.sync_complete)
                        .setMessage(message?.takeIf { it.isNotBlank() } ?: getString(R.string.sync_complete_default))
                        .setPositiveButton(android.R.string.ok, null)
                        .show()
                }.onFailure {
                    AlertDialog.Builder(this)
                        .setTitle(R.string.error)
                        .setMessage(it.localizedMessage ?: getString(R.string.sync_failed_default))
                        .setPositiveButton(android.R.string.ok, null)
                        .show()
                }
            }
        }.start()
    }

    private fun setSyncInProgress(inProgress: Boolean) {
        isSyncing = inProgress
        binding.syncProgressContainer.visibility = if (inProgress) View.VISIBLE else View.GONE
        binding.navView.menu.findItem(R.id.nav_sync_now)?.apply {
            isEnabled = !inProgress
            title = getString(if (inProgress) R.string.syncing else R.string.sync_now)
        }
    }

    private fun exportKeybox() {
        pendingExportData = kbFile.serialize()
        if (pendingExportData == null) {
            AlertDialog.Builder(this)
                .setTitle(R.string.error)
                .setMessage(R.string.save_keybox_failed)
                .setPositiveButton(android.R.string.ok, null)
                .show()
            return
        }

        val intent = Intent(Intent.ACTION_CREATE_DOCUMENT).apply {
            addCategory(Intent.CATEGORY_OPENABLE)
            type = "application/octet-stream"
            putExtra(Intent.EXTRA_TITLE, KEYBOX_FILE_NAME)
        }
        exportLauncher.launch(intent)
    }

    private fun writeExportedKeybox(uri: Uri) {
        val data = pendingExportData ?: return
        runCatching {
            contentResolver.openOutputStream(uri)?.use { it.write(data) }
                ?: error(getString(R.string.export_keybox_failed))
        }.onSuccess {
            AlertDialog.Builder(this)
                .setTitle(R.string.success)
                .setMessage(R.string.export_keybox_success)
                .setPositiveButton(android.R.string.ok, null)
                .show()
        }.onFailure {
            AlertDialog.Builder(this)
                .setTitle(R.string.error)
                .setMessage(getString(R.string.export_keybox_failed_with_message, it.localizedMessage ?: "unknown error"))
                .setPositiveButton(android.R.string.ok, null)
                .show()
        }
    }

    private fun importKeybox() {
        val intent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
            addCategory(Intent.CATEGORY_OPENABLE)
            type = "*/*"
        }
        importLauncher.launch(intent)
    }

    private fun readImportedKeybox(uri: Uri) {
        runCatching {
            contentResolver.openInputStream(uri)?.use { it.readBytes() }
                ?: error(getString(R.string.import_keybox_failed))
        }.onSuccess { data ->
            promptImportPassword(data)
        }.onFailure {
            AlertDialog.Builder(this)
                .setTitle(R.string.error)
                .setMessage(getString(R.string.import_keybox_failed_with_message, it.localizedMessage ?: "unknown error"))
                .setPositiveButton(android.R.string.ok, null)
                .show()
        }
    }

    private fun promptImportPassword(data: ByteArray) {
        val input = EditText(this).apply {
            hint = getString(R.string.master_password)
            inputType = android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_PASSWORD
        }
        AlertDialog.Builder(this)
            .setTitle(R.string.import_keybox)
            .setView(input)
            .setNegativeButton(android.R.string.cancel, null)
            .setPositiveButton(R.string.unlock_and_load) { _, _ ->
                if (!loadDownloadedKeybox(data, input.text?.toString().orEmpty())) {
                    AlertDialog.Builder(this)
                        .setTitle(R.string.error)
                        .setMessage(R.string.import_keybox_unlock_failed)
                        .setPositiveButton(android.R.string.ok, null)
                        .show()
                }
            }
            .show()
    }
}
